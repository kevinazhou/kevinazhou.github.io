#include "Door.h"
#include "Game.h"

void Door::Setup(std::string dest, DoorState s, DoorDirection d)
{
	destination = dest;
	state = s;
	direction = d;

	if (direction == DoorDirection::Up || direction == DoorDirection::Down)
	{
		collision->SetSize(30, 64);
	}
	else if (direction == DoorDirection::Left || direction == DoorDirection::Right)
	{
		collision->SetSize(64, 30);
	}

	UpdateTexture();
}

void Door::OnUpdate(float deltaTime)
{
	UpdateTexture();
}

void Door::UpdateTexture()
{
	if (direction == DoorDirection::Up)
	{
		if (state == DoorState::Closed)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/UpClosed.png"));
		}
		else if (state == DoorState::Locked)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/UpLocked.png"));
		}
		else if (state == DoorState::Open)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/UpOpen.png"));
		}
	}
	else if (direction == DoorDirection::Down)
	{
		if (state == DoorState::Closed)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/DownClosed.png"));
		}
		else if (state == DoorState::Locked)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/DownLocked.png"));
		}
		else if (state == DoorState::Open)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/DownOpen.png"));
		}
	}
	else if (direction == DoorDirection::Left)
	{
		if (state == DoorState::Closed)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/LeftClosed.png"));
		}
		else if (state == DoorState::Locked)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/LeftLocked.png"));
		}
		else if (state == DoorState::Open)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/LeftOpen.png"));
		}
	}
	else if (direction == DoorDirection::Right)
	{
		if (state == DoorState::Closed)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/RightClosed.png"));
		}
		else if (state == DoorState::Locked)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/RightLocked.png"));
		}
		else if (state == DoorState::Open)
		{
			sprite->SetTexture(GetGame()->GetTexture("Assets/Door/RightOpen.png"));
		}
	}
}

std::string Door::GetDestination()
{
	return destination;
}

DoorState Door::GetDoorState()
{
	return state;
}

void Door::SetDoorState(DoorState s)
{
	state = s;
}

DoorDirection Door::GetDoorDirection()
{
	return direction;
}