
// BlockBed.cpp

#include "Globals.h"
#include "BlockBed.h"

#include "BroadcastInterface.h"
#include "../Entities/Player.h"
#include "../World.h"
#include "../BoundingBox.h"
#include "../Mobs/Monster.h"
#include "../BlockEntities/BedEntity.h"





void cBlockBedHandler::OnBroken(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, Vector3i a_BlockPos, BLOCKTYPE a_OldBlockType, NIBBLETYPE a_OldBlockMeta)
{
	auto Direction = MetaDataToDirection(a_OldBlockMeta & 0x03);
	if ((a_OldBlockMeta & 0x08) != 0)
	{
		// Was pillow
		if (a_ChunkInterface.GetBlock(a_BlockPos - Direction) == E_BLOCK_BED)
		{
			// First replace the bed with air
			a_ChunkInterface.FastSetBlock(a_BlockPos - Direction, E_BLOCK_AIR, 0);

			// Then destroy the bed entity
			Vector3i PillowPos(a_BlockPos - Direction);
			a_ChunkInterface.SetBlock(PillowPos, E_BLOCK_AIR, 0);
		}
	}
	else
	{
		// Was foot end
		if (a_ChunkInterface.GetBlock(a_BlockPos + Direction) == E_BLOCK_BED)
		{
			// First replace the bed with air
			a_ChunkInterface.FastSetBlock(a_BlockPos + Direction, E_BLOCK_AIR, 0);

			// Then destroy the bed entity
			Vector3i FootPos(a_BlockPos + Direction);
			a_ChunkInterface.SetBlock(FootPos, E_BLOCK_AIR, 0);
		}
	}
}





bool cBlockBedHandler::OnUse(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer & a_Player, int a_BlockX, int a_BlockY, int a_BlockZ, eBlockFace a_BlockFace, int a_CursorX, int a_CursorY, int a_CursorZ)
{
	Vector3i Coords(a_BlockX, a_BlockY, a_BlockZ);
	NIBBLETYPE Meta = a_ChunkInterface.GetBlockMeta(Coords);
	auto PillowDirection = MetaDataToDirection(Meta & 0x03);
	auto OffsetCoords = Coords + PillowDirection;

	if ((Meta & 0x08) != 0)
	{
		OffsetCoords = Coords - PillowDirection;
	}

	if (a_WorldInterface.GetDimension() != dimOverworld)
	{
		a_WorldInterface.DoExplosionAt(5, a_BlockX, a_BlockY, a_BlockZ, true, esBed, &Coords);
	}
	else if (
		!(((a_WorldInterface.GetTimeOfDay() > 12541) && (a_WorldInterface.GetTimeOfDay() < 23458)) ||
		(a_Player.GetWorld()->GetWeather() == wThunderstorm))
	)  // Source: https://minecraft.gamepedia.com/Bed#Sleeping
	{
		a_Player.SendAboveActionBarMessage("You can only sleep at night and during thunderstorms");

		if ((a_Player.GetLastBedPos() != Coords) && (a_Player.GetLastBedPos() != OffsetCoords))
		{
			a_Player.SetBedPos(Coords);
			a_Player.SendMessageSuccess("Home position set successfully");
		}
	}
	else
	{
		TrySleeping(a_ChunkInterface, a_WorldInterface, a_Player, Coords, OffsetCoords, Meta, PillowDirection);
	}
	return true;
}





void cBlockBedHandler::OnPlacedByPlayer(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer & a_Player, const sSetBlock & a_BlockChange)
{
	a_Player.GetWorld()->DoWithBedAt(a_BlockChange.GetX(), a_BlockChange.GetY(), a_BlockChange.GetZ(), [&](cBedEntity & a_Bed)
		{
			a_Bed.SetColor(a_Player.GetEquippedItem().m_ItemDamage);
			return true;
		}
	);
}





void cBlockBedHandler::TrySleeping(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer & a_Player, Vector3i a_Coords, Vector3i a_OffsetCoords, NIBBLETYPE a_Meta, Vector3i a_PillowDirection)
{
	if ((a_Meta & 0x4) == 0x4)
	{
		a_Player.SendMessageFailure("This bed is occupied");
	}
	else
	{
		auto FindMobs = [](cEntity & a_Entity)
		{
			return (
				(a_Entity.GetEntityType() == cEntity::etMonster) &&
				(static_cast<cMonster&>(a_Entity).GetMobFamily() == cMonster::mfHostile)
			);
		};

		if (!a_Player.GetWorld()->ForEachEntityInBox(cBoundingBox(a_Player.GetPosition() - Vector3i(0, 5, 0), 8, 10), FindMobs))
		{
			a_Player.SendAboveActionBarMessage("You may not rest now, there are monsters nearby");
		}
		else
		{
			SleepInBed(a_ChunkInterface, a_WorldInterface, a_Player, a_Coords, a_OffsetCoords, a_Meta, a_PillowDirection);
		}
	}
}





void cBlockBedHandler::SleepInBed(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer & a_Player, Vector3i a_Coords, Vector3i a_OffsetCoords, NIBBLETYPE a_Meta, Vector3i a_PillowDirection)
{
	if ((a_Meta & 0x8) == 0x8)
	{
		// Is pillow
		a_WorldInterface.GetBroadcastManager().BroadcastUseBed(a_Player, a_Coords);
	}
	else
	{
		// Is foot end
		VERIFY((a_Meta & 0x4) != 0x4);  // Occupied flag should never be set, else our compilator (intended) is broken

		if (a_ChunkInterface.GetBlock(a_Coords + a_PillowDirection) == E_BLOCK_BED)  // Must always use pillow location for sleeping
		{
			a_WorldInterface.GetBroadcastManager().BroadcastUseBed(a_Player, a_Coords + a_PillowDirection);
		}
	}

	SetBedOccupationState(a_ChunkInterface, a_Player.GetLastBedPos(), true);
	a_Player.SetIsInBed(true);

	if ((a_Player.GetLastBedPos() != a_Coords) && (a_Player.GetLastBedPos() != a_OffsetCoords))
	{
		a_Player.SetBedPos(a_Coords);
		a_Player.SendMessageSuccess("Home position set successfully");
	}

	auto TimeFastForwardTester = [](cPlayer & a_OtherPlayer)
	{
		if (!a_OtherPlayer.IsInBed())
		{
			return true;
		}
		return false;
	};

	if (a_WorldInterface.ForEachPlayer(TimeFastForwardTester))
	{
		a_WorldInterface.ForEachPlayer([&](cPlayer & a_OtherPlayer)
			{
				cBlockBedHandler::SetBedOccupationState(a_ChunkInterface, a_OtherPlayer.GetLastBedPos(), false);
				a_OtherPlayer.SetIsInBed(false);
				return false;
			}
		);
		a_WorldInterface.SetTimeOfDay(0);

		if (a_Player.GetWorld()->GetWeather() == wThunderstorm)
		{
			a_Player.GetWorld()->SetWeather(wSunny);
		}

		a_ChunkInterface.SetBlockMeta(a_Coords, a_Meta & 0x0b);  // Clear the "occupied" bit of the bed's block
	}
}





cItems cBlockBedHandler::ConvertToPickups(NIBBLETYPE a_BlockMeta, cBlockEntity * a_BlockEntity, const cEntity * a_Digger, const cItem * a_Tool)
{
	short color = E_META_WOOL_RED;
	if (a_BlockEntity != nullptr)
	{
		color = reinterpret_cast<cBedEntity *>(a_BlockEntity)->GetColor();
	}
	return cItem(E_ITEM_BED, 1, color);
}
