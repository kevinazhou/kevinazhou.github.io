#include "LaserMine.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Game.h"
#include "LaserComponent.h"

LaserMine::LaserMine(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/LaserMine.gpmesh"));

	laser = new LaserComponent(this);
}