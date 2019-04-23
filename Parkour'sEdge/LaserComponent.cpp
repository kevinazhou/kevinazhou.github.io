#include "LaserComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "Player.h"
#include "Block.h"

LaserComponent::LaserComponent(Actor* owner)
	:MeshComponent(owner, true)
{
	SetMesh(mOwner->GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
	lines.clear();
	LineSegment line;
	line.mStart = mOwner->GetWorldPosition();
	line.mEnd = mOwner->GetWorldPosition() + (mOwner->GetWorldForward() * 500);
	lines.push_back(line);

	CastInfo info;
	if (SegmentCast(mOwner->GetGame()->GetPlayer(), lines[0], info))
	{//if does intersect with player
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav"), 0);
		lines[0].mEnd = info.mPoint;
		Player* p = mOwner->GetGame()->GetPlayer();
		p->SetPosition(p->GetRespawnLocation());
	}
	else
	{
		CastInfo info2;
		if (SegmentCast(mOwner->GetGame()->GetBlockVec(), lines[0], info2, mOwner->GetParent()))
		{//first beam
			lines[0].mEnd = info2.mPoint;//end of first beam
			
			int i = 0;
			Block* collidedWith = static_cast<Block*>(info2.mActor);
			while (collidedWith->GetMirrorStatus())
			{//if hitting mirror
				LineSegment linne;
				linne.mStart = info2.mPoint;//setting start of reflected beam

				Vector3 VEC = lines[i].mEnd - lines[i].mStart;
				VEC.Normalize();
				Vector3 reflectedVec = Vector3::Reflect(VEC, info2.mNormal);
				linne.mEnd = linne.mStart + (reflectedVec * 500);//setting end of reflected

				lines.push_back(linne);

				if (SegmentCast(mOwner->GetGame()->GetBlockVec(), lines[i + 1], info2, collidedWith))
				{
					collidedWith = static_cast<Block*>(info2.mActor);
				}
				else 
				{
					break;
				}
				i++;
			}
		}
	}

}


void LaserComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform
		
		for (size_t i = 0; i < lines.size(); i++)
		{
			shader->SetMatrixUniform("uWorldTransform",
				TransformHelper(lines[i]));


			// Set the active texture
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw

			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
		
	}
}

Matrix4 LaserComponent::TransformHelper(LineSegment li)
{
	float length = li.Length();
	Matrix4 scale = Matrix4::CreateScale(1.0f);
	scale.mat[0][0] = length;

	
	Vector3 vec1 = Vector3::UnitX;
	Vector3 vec2 = li.mEnd - li.mStart;
	vec2.Normalize();
	float angle = acos(Vector3::Dot(vec1, vec2));

	Quaternion q;//identity quaternion
	if (Vector3::Dot(vec1, vec2) < 0.99f || Vector3::Dot(vec1, vec2) > 1.01f)
	{
		Vector3 axis = Vector3::Cross(vec1, vec2);
		axis.Normalize();
		q = Quaternion(axis, angle);
	}
	if (Vector3::Dot(vec1, vec2) == -1.0f)
	{
		q = Quaternion(Vector3::UnitZ, Math::Pi);
	}
	Matrix4 rotate = Matrix4::CreateFromQuaternion(q);
	

	Vector3 halfway = li.PointOnSegment(0.5);
	Matrix4 translate = Matrix4::CreateTranslation(halfway);

	Matrix4 ret = scale * rotate * translate;
	 
	return ret;

}