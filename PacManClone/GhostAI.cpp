#include "GhostAI.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "PathNode.h"
#include "AnimatedSprite.h"
#include <SDL/SDL.h>
#include <unordered_map>
#include "Ghost.h"
#include "PacMan.h"
#include "Random.h"

GhostAI::GhostAI(class Actor* owner)
:Component(owner, 50)
{
	mGhost = static_cast<Ghost*>(owner);
}

void GhostAI::Update(float deltaTime)
{
	Vector2 pos = mOwner->GetPosition();

	AnimatedSprite* s = mOwner->GetComponent<AnimatedSprite>();

	if (mNextNode->GetPosition().x == mPrevNode->GetPosition().x)
	{//up or down direction
		if (mNextNode->GetPosition().y < mPrevNode->GetPosition().y)
		{//going up
			direction = Vector2(0, -1);
			s->SetAnimation("up");
			if (GetState() == State::Dead)
			{
				s->SetAnimation("deadup");
			}
		}
		else
		{
			direction = Vector2(0, 1);
			s->SetAnimation("down");
			if (GetState() == State::Dead)
			{
				s->SetAnimation("deaddown");
			}
		}
	}
	else
	{//left or right
		if (mNextNode->GetPosition().x < mPrevNode->GetPosition().x)
		{//going left
			direction = Vector2(-1, 0);
			s->SetAnimation("left");
			if (GetState() == State::Dead)
			{
				s->SetAnimation("deadleft");
			}
		}
		else
		{
			direction = Vector2(1, 0);
			s->SetAnimation("right");
			if (GetState() == State::Dead)
			{
				s->SetAnimation("deadright");
			}
		}
	}

	if (GetState() == State::Dead)
	{
		Vector2 change = direction * 125.0f * deltaTime;
		pos += change;
		mOwner->SetPosition(pos);

		CollisionComponent* ghostCollision = mOwner->GetComponent<CollisionComponent>();

		Vector2 offset = Vector2::Zero;
		CollisionComponent* nextCollision = mNextNode->GetComponent<CollisionComponent>();
		CollSide side = ghostCollision->GetMinOverlap(nextCollision, offset);
		if (side != CollSide::None)
		{//made it to the next node
			mOwner->SetPosition(mNextNode->GetPosition());
			if (mNextNode == mOwner->GetGame()->mGhostPen)
			{//when it reaches the pen
				Start(mOwner->GetGame()->mGhostPen);
			}
			else
			{
				if (!noDeadPathFound)//if there is a path back to the pen
				{
					if (mNextNode->GetType() == PathNode::Type::Tunnel)
					{//if reaching a tunnel
						mOwner->SetPosition(mPath.back()->GetPosition());//teleporting to other tunnel
						if (!mPath.empty())
						{
							mPrevNode = mPath.back();//tunnel is now prev
							mPath.pop_back();
							mNextNode = mPath.back();
							mPath.pop_back();
						}
					}
					else
					{
						mPrevNode = mNextNode;
						if (!mPath.empty())
						{
							mNextNode = mPath.back();
							mPath.pop_back();
						}
					}
				}
				else
				{//if no path to pen is found
					if (mNextNode->GetType() == PathNode::Type::Tunnel)
					{//if reaching a tunnel
						if (mNextNode == mOwner->GetGame()->mTunnelLeft)
						{//left tunnel
							mOwner->SetPosition(mOwner->GetGame()->mTunnelRight->GetPosition());//teleporting to other tunnel
							mNextNode = mOwner->GetGame()->mTunnelRight;
							mPrevNode = mOwner->GetGame()->mTunnelLeft;
						}
						else
						{
							mOwner->SetPosition(mOwner->GetGame()->mTunnelLeft->GetPosition());//teleporting to other tunnel
							mNextNode = mOwner->GetGame()->mTunnelLeft;
							mPrevNode = mOwner->GetGame()->mTunnelRight;
						}
					}

					if (!AStar(mNextNode, mOwner->GetGame()->mGhostPen))
					{//mNextNode is colliding node
						noDeadPathFound = true;

						std::vector<PathNode*> adj = mNextNode->mAdjacent;
						size_t ind = 0;
						PathNode* anotherNode = adj[ind];
						while (anotherNode == mPrevNode)
						{
							ind++;
							anotherNode = adj[ind];
						}//keep going to neighboring nodes until a path is found

						mPrevNode = mNextNode;
						mNextNode = anotherNode;

					}
					else
					{
						noDeadPathFound = false;
					}
				}
			}
			
		}

	}
	else if (GetState() == State::Frightened)
	{//frightened bahavior
		if (SDL_GetTicks() - timeFrightened < 5000)
		{
			s->SetAnimation("scared0");
		}
		else
		{
			s->SetAnimation("scared1");
		}
		//putting direction stuff here makes game freeze when frightened ghosts go through tunnel
		Vector2 change = direction * 65.0f * deltaTime;
		pos += change;
		mOwner->SetPosition(pos);

		CollisionComponent* ghostCollision = mOwner->GetComponent<CollisionComponent>();

		Vector2 offset = Vector2::Zero;
		CollisionComponent* nextCollision = mNextNode->GetComponent<CollisionComponent>();
		CollSide side = ghostCollision->GetMinOverlap(nextCollision, offset);
		if (side != CollSide::None)
		{//made it to the next node
			mOwner->SetPosition(mNextNode->GetPosition());
			PathNode* forbidden = mPrevNode;
			if (mNextNode->GetType() == PathNode::Type::Tunnel)
			{//if reaching a tunnel
				if (mNextNode == mOwner->GetGame()->mTunnelLeft)
				{
					mOwner->SetPosition(mOwner->GetGame()->mTunnelRight->GetPosition());
					mPrevNode = mOwner->GetGame()->mTunnelRight;
					forbidden = mOwner->GetGame()->mTunnelLeft;
				}
				else
				{
					mOwner->SetPosition(mOwner->GetGame()->mTunnelLeft->GetPosition());
					mPrevNode = mOwner->GetGame()->mTunnelLeft;
					forbidden = mOwner->GetGame()->mTunnelRight;
				}
			}
			else
			{
				mPrevNode = mNextNode;
			}

			std::vector<PathNode*> adj = mPrevNode->mAdjacent;
			int randRange = Random::GetIntRange(0, adj.size() * 1000);
			int rand = randRange % adj.size();
			mNextNode = adj[rand];
			while (mNextNode == forbidden || mNextNode->GetType() == PathNode::Type::Ghost)
			{
				randRange = Random::GetIntRange(0, adj.size() * 1000);
				rand = randRange % adj.size();
				mNextNode = adj[rand];
			}
		}

		if (SDL_GetTicks() - timeFrightened > 7000)
		{
			mState = State::Scatter;
			timeScattered = SDL_GetTicks();
		}
	}

	else
	{
		Vector2 change = direction * 90.0f * deltaTime;
		pos += change;
		mOwner->SetPosition(pos);

		if (GetState() == State::Chase)
		{
			CollisionComponent* ghostCollision = mOwner->GetComponent<CollisionComponent>();

			Vector2 offset = Vector2::Zero;
			CollisionComponent* nextCollision = mNextNode->GetComponent<CollisionComponent>();
			CollSide side = ghostCollision->GetMinOverlap(nextCollision, offset);
			if (side != CollSide::None)
			{//made it to the next node
				//no guarantees that the path is full
				mOwner->SetPosition(mNextNode->GetPosition());
				PathNode* target = GetChaseTarget();
				PathNode* forbidden = mPrevNode;
		

				if (mNextNode->GetType() == PathNode::Type::Tunnel)
				{//if reaching a tunnel
					if (mNextNode == mOwner->GetGame()->mTunnelLeft)
					{//teleport to right tunnel
						mOwner->SetPosition(mOwner->GetGame()->mTunnelRight->GetPosition());
						mPrevNode = mOwner->GetGame()->mTunnelLeft;
						mNextNode = mOwner->GetGame()->mTunnelRight;
						forbidden = mOwner->GetGame()->mTunnelLeft;
					}
					else
					{
						mOwner->SetPosition(mOwner->GetGame()->mTunnelLeft->GetPosition());
						mPrevNode = mOwner->GetGame()->mTunnelRight;
						mNextNode = mOwner->GetGame()->mTunnelLeft;
						forbidden = mOwner->GetGame()->mTunnelRight;
					}

				}

				//should automatically set nodes
				//if AStar works, current node is set as mPrevNode
				//and mNextNode is set automatically
				if (!AStar(mNextNode, target))
				{//failsafe
					//if AStar fails, mNextNode is still the node you're on
					//mPrevNode is still node you want to avoid
					//need to then manually assign mNextNode and mPrevNode
					std::vector<PathNode*> adj = mNextNode->mAdjacent;
					
					PathNode* closestNode = adj[0];
					if (closestNode == forbidden)
					{
						closestNode = adj[1];
					}
					Vector2 nodeVec = closestNode->GetPosition();
					Vector2 distVec = target->GetPosition() - nodeVec;
					float closestNodeDist = distVec.Length();
					for (size_t k = 1; k < adj.size(); k++)
					{
						PathNode* nodeBeingAnalyzed = adj[k];
						if (nodeBeingAnalyzed != forbidden)
						{
							nodeVec = nodeBeingAnalyzed->GetPosition();
							distVec = target->GetPosition() - nodeVec;
							if (closestNodeDist > distVec.Length())
							{
								closestNode = nodeBeingAnalyzed;
								closestNodeDist = distVec.Length();
							}
						}
					}//now closestNode should have the node closest to target

					mPrevNode = mNextNode;
					mNextNode = closestNode;
				}


			}


			if (SDL_GetTicks() - timeChased > 20000)
			{
				mState = State::Scatter;
				timeScattered = SDL_GetTicks();
			}
		}
		else if (GetState() == State::Scatter)
		{
			CollisionComponent* ghostCollision = mOwner->GetComponent<CollisionComponent>();

			Vector2 offset = Vector2::Zero;
			CollisionComponent* nextCollision = mNextNode->GetComponent<CollisionComponent>();
			CollSide side = ghostCollision->GetMinOverlap(nextCollision, offset);
			if (side != CollSide::None)
			{//made it to the next node
				mOwner->SetPosition(mNextNode->GetPosition());
				if (mPath.empty())//circle arround scatter node
				{
					if (!AStar(mNextNode, mGhost->GetScatterNode()))
					{//if it can't make a valid path
						Vector2 scatterPos = mGhost->GetScatterNode()->GetPosition();
						std::vector<PathNode*> adj = mNextNode->mAdjacent;
						PathNode* closestToScatter = adj[0];
						if (closestToScatter == mPrevNode)
						{
							closestToScatter = adj[1];
						}
						Vector2 firstPos = closestToScatter->GetPosition();
						Vector2 distVec = scatterPos - firstPos;
						float min = distVec.Length();

						for (size_t i = 1; i < adj.size(); i++)
						{//find out which is adjacent node is the closest to scatter
							if (adj[i] != mPrevNode)
							{
								Vector2 thisPos = adj[i]->GetPosition();
								distVec = scatterPos - thisPos;
								if (distVec.Length() < min)
								{
									min = distVec.Length();
									closestToScatter = adj[i];
								}
							}
						}
						mPrevNode = mNextNode;
						mNextNode = closestToScatter;
					}
				}
				else
				{
					if (mNextNode->GetType() == PathNode::Type::Tunnel)
					{//if reaching a tunnel
						mOwner->SetPosition(mPath.back()->GetPosition());//teleporting to other tunnel
						if (!mPath.empty())
						{
							mPrevNode = mPath.back();//tunnel is now prev
							mPath.pop_back();
							mNextNode = mPath.back();
							mPath.pop_back();
						}
					}
					else
					{
						mPrevNode = mNextNode;
						if (!mPath.empty())
						{
							mNextNode = mPath.back();
							mPath.pop_back();
						}
					}
				}

				if (SDL_GetTicks() - timeScattered > 5000)
				{
					mState = State::Chase;
					timeChased = SDL_GetTicks();


			}

			
			}

		}
	}
	
}

PathNode* GhostAI::GetChaseTarget()
{
	PathNode* targetNode = nullptr;
	PacMan* pac = (PacMan*)mOwner->GetGame()->mPlayer;
	if (mGhost->GetType() == Ghost::Type::Blinky)
	{
		targetNode = pac->GetPrevNode();
	}
	else if (mGhost->GetType() == Ghost::Type::Pinky)
	{//target path node closest to 80 units in front of pacman
		Vector2 eighty = pac->GetPointInFrontOf(80.0);
		std::vector<PathNode*> nodes = mOwner->GetGame()->mPathNodes;

		PathNode* closestNode = nodes[0];
		if (closestNode == mPrevNode)
		{
			closestNode = nodes[1];
		}
		Vector2 nodeVec = closestNode->GetPosition();
		Vector2 distVec = eighty - nodeVec;
		float closestNodeDist = distVec.Length();
		for (size_t k = 1; k < nodes.size(); k++)
		{
			PathNode* nodeBeingAnalyzed = nodes[k];
			if (nodeBeingAnalyzed != mPrevNode)
			{
				nodeVec = nodeBeingAnalyzed->GetPosition();
				distVec = eighty - nodeVec;
				if (closestNodeDist > distVec.Length())
				{
					closestNode = nodeBeingAnalyzed;
				}
			}
		}//now closestNode should have the node closest to 80 units in front

		targetNode = closestNode;
	}
	else if (mGhost->GetType() == Ghost::Type::Inky)
	{
		//get point 40 units in front of pacman
		Vector2 forty = pac->GetPointInFrontOf(40.0);

		//make a vector from blinky's position to this point
		Vector2 blinkyPos = mOwner->GetGame()->mGhosts[0]->GetPosition();
		Vector2 v = forty - blinkyPos;

		//double length of v, add it to blinky's position to get point q
		v *= 2;
		Vector2 q = blinkyPos + v;

		//find pathNode closest to q
		std::vector<PathNode*> nodes = mOwner->GetGame()->mPathNodes;
		PathNode* closestNode = nodes[0];
		if (closestNode == mPrevNode)
		{
			closestNode = nodes[1];
		}
		Vector2 nodeVec = closestNode->GetPosition();
		Vector2 distVec = q - nodeVec;
		float closestNodeDist = distVec.Length();
		for (size_t k = 1; k < nodes.size(); k++)
		{
			PathNode* nodeBeingAnalyzed = nodes[k];
			if (nodeBeingAnalyzed != mPrevNode)
			{
				nodeVec = nodeBeingAnalyzed->GetPosition();
				distVec = q - nodeVec;
				if (closestNodeDist > distVec.Length())
				{
					closestNode = nodeBeingAnalyzed;
				}
			}
		}//now closestNode should have the node closest to q

		targetNode = closestNode;
	}
	else if (mGhost->GetType() == Ghost::Type::Clyde)
	{//target pacman's previous node if clyde is < 150 from pacman
		Vector2 pacPos = pac->GetPosition();
		Vector2 dist = pacPos - mOwner->GetPosition();

		if (dist.Length() < 150)
		{
			targetNode = pac->GetPrevNode();
		}
		else
		{//otherwise target scatterNode
			targetNode = mGhost->GetScatterNode();
		}
	}

	return targetNode;
}


void GhostAI::Frighten()
{
	if (GetState() != State::Dead)
	{
		mState = State::Frightened;
		if (direction.x == -1)//going left
		{
			direction = Vector2(1, 0);
		}
		else if (direction.x == 1)//going right
		{
			direction = Vector2(-1, 0);
		}
		else if (direction.y == -1)//going up
		{
			direction = Vector2(0, 1);
		}
		else if (direction.x == 1)//going down
		{
			direction = Vector2(0, -1);
		}

		timeFrightened = SDL_GetTicks();

		PathNode* temp = mNextNode;
		mNextNode = mPrevNode;
		mPrevNode = temp;
		mPath.clear();
	}
}

void GhostAI::Start(PathNode* startNode)
{
	mOwner->SetPosition(startNode->GetPosition());
	mState = State::Scatter;
	timeScattered = SDL_GetTicks();
	mPrevNode = nullptr;
	mNextNode = nullptr;
	mTargetNode = nullptr;
	AStar(startNode, mGhost->GetScatterNode());

}

float GhostAI::CalculateHeuristic(PathNode* currNode, PathNode* goalNode)
{
	float normal, leftTunnel, rightTunnel;

	Vector2 currPos = currNode->GetPosition();
	Vector2 goalPos = goalNode->GetPosition();

	Vector2 distVec;//vector between 2 positions
	distVec = currPos - goalPos;

	normal = distVec.Length();

	Vector2 leftTunnelNodePos = mOwner->GetGame()->mTunnelLeft->GetPosition();

	distVec = currPos - leftTunnelNodePos;

	Vector2 vecFromTunnelsToGoal = leftTunnelNodePos - goalPos;

	leftTunnel = distVec.Length();
	leftTunnel += vecFromTunnelsToGoal.Length();

	//now righttunnel distance

	Vector2 rightTunnelNodePos = mOwner->GetGame()->mTunnelRight->GetPosition();
	distVec = currPos = rightTunnelNodePos;

	vecFromTunnelsToGoal = rightTunnelNodePos - goalPos;

	rightTunnel = distVec.Length();
	rightTunnel += vecFromTunnelsToGoal.Length();


	return std::min(normal, std::min(leftTunnel, rightTunnel));
}

float GhostAI::CalculateG(PathNode* currNode, PathNode* adjNode)
{
	if (currNode->GetType() == PathNode::Type::Tunnel && adjNode->GetType() == PathNode::Type::Tunnel)
	{
		return 0.0f;
	}
	else
	{
		Vector2 currPos = currNode->GetPosition();
		Vector2 adjPos = adjNode->GetPosition();

		Vector2 vec = currPos - adjPos;

		return vec.Length();
	}
}

bool GhostAI::AStar(PathNode* startNode, PathNode* goalNode)
{
	//f total function
	//g actual cost from start to current
	//h heuristic
	mPath.clear();
	PathNode* currentNode = startNode;
	std::unordered_map<PathNode*, PathNode*> openSet;
	std::unordered_map<PathNode*, PathNode*> closedSet;

	std::unordered_map<PathNode*, float> gCosts;
	std::unordered_map<PathNode*, float> hCosts;
	std::unordered_map<PathNode*, float> fCosts;
	std::unordered_map<PathNode*, PathNode*> parents;

	gCosts.insert({ currentNode, 0.0f });//start node g value is 0

	closedSet.insert({ currentNode, currentNode });//closed set starts with start node
	
	do
	{
		std::vector<PathNode*> adjacents = currentNode->mAdjacent;
		for (size_t i = 0; i < adjacents.size(); i++)
		{
			if (closedSet.find(adjacents[i]) != closedSet.end())
			{//it is in closed set
				continue;
			}
			else if (openSet.find(adjacents[i]) != openSet.end())
			{//check to adopt
				float newGVal = CalculateG(currentNode, adjacents[i]);
				if (newGVal < gCosts[adjacents[i]])
				{
					parents[adjacents[i]] = currentNode;
					gCosts[adjacents[i]] = newGVal;
					fCosts[adjacents[i]] = newGVal + hCosts[adjacents[i]];
				}
			}
			else if(adjacents[i] != mPrevNode)//so the ghost doesn't turn around
			{
				parents.insert({ adjacents[i], currentNode });//setting the parent
				float hValue, gValue, fValue;
				if (hCosts.find(adjacents[i]) != hCosts.end())//if heuristic already been calculated
				{
					hValue = hCosts.find(adjacents[i])->second;
				}
				else//if have to calculate heuristic first time
				{
					hValue = CalculateHeuristic(adjacents[i], goalNode);
					hCosts.insert({ adjacents[i], hValue});
				}
				
				gValue = CalculateG(currentNode, adjacents[i]);
				gCosts.insert({ adjacents[i], gValue });//should always be updating g values

				fValue = gValue + hValue;
				fCosts.insert({ adjacents[i], fValue });
				openSet.insert({ adjacents[i], adjacents[i] });
			}
		}

		if (openSet.empty())
		{
			break;
		}

		//finding smallest fValue node
		PathNode* lowestNode = nullptr;
		size_t first = 0;
		for (std::pair<PathNode*, PathNode*> element : openSet)
		{
			if (first == 0)
			{
				lowestNode = element.first;
			}
			else
			{
				if (fCosts[lowestNode] > fCosts[element.first])
				{
					lowestNode = element.first;
				}
			}
			first++;
		}
		currentNode = lowestNode;

		//moving currentNode from openSet to closedSet
		if (openSet.find(currentNode) != openSet.end())
		{
			openSet.erase(currentNode);
		}
		closedSet.insert({ currentNode, currentNode });

	} while (currentNode != goalNode);

	if (currentNode != goalNode)//path not found
	{
		mTargetNode = goalNode;
		mPath.clear();
		return false;
	}
	else//path found
	{
		mTargetNode = goalNode;
		mPrevNode = startNode;

		PathNode* temp = goalNode;
		mPath.push_back(temp);
		while (mPath.back() != startNode)
		{
			mPath.push_back(parents[temp]);
			temp = parents[temp];
		}//now full path is in mPath
		mPath.pop_back();//erasing start from mPath
		
		mNextNode = mPath.back();

		mPath.pop_back();//erasing mNextNode from mPath
		
		return true;
	}
}



void GhostAI::Die()
{
	mState = State::Dead;
	noDeadPathFound = true;
	//doesn't calculate a path to pen until reaching another PathNode first
}

void GhostAI::DebugDrawPath(SDL_Renderer* render)
{
	// Draw a rectangle at the target node
	if (mTargetNode != nullptr)
	{
		const int SIZE = 16;
		SDL_Rect r;
		r.x = static_cast<int>(mTargetNode->GetPosition().x) - SIZE / 2;
		r.y = static_cast<int>(mTargetNode->GetPosition().y) - SIZE / 2;
		r.w = SIZE;
		r.h = SIZE;
		SDL_RenderDrawRect(render, &r);
	}

	// Line from ghost to next node
	if (mNextNode != nullptr)
	{
		SDL_RenderDrawLine(render,
			static_cast<int>(mOwner->GetPosition().x),
			static_cast<int>(mOwner->GetPosition().y),
			static_cast<int>(mNextNode->GetPosition().x),
			static_cast<int>(mNextNode->GetPosition().y));
	}

	// Exit if no path
	if (mPath.empty())
	{
		return;
	}

	// Line from next node to subsequent on path
	SDL_RenderDrawLine(render,
		static_cast<int>(mNextNode->GetPosition().x),
		static_cast<int>(mNextNode->GetPosition().y),
		static_cast<int>(mPath.back()->GetPosition().x),
		static_cast<int>(mPath.back()->GetPosition().y));

	// Lines for rest of path
	for (size_t i = 0; i < mPath.size() - 1; i++)
	{
		SDL_RenderDrawLine(render,
			static_cast<int>(mPath[i]->GetPosition().x),
			static_cast<int>(mPath[i]->GetPosition().y),
			static_cast<int>(mPath[i + 1]->GetPosition().x),
			static_cast<int>(mPath[i + 1]->GetPosition().y));
	}
}
