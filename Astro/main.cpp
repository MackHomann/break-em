#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>;
//using namespace std;

// Override base class with your custom functionality

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		// Name your application
		sAppName = "BrickGame";
	}

private:
	float playerPos		= 10.0f;
	float playerWidth	= 2.0f;
	float playerSpeed	= 14.0f;

	struct oBall {
		olc::vf2d pos	= { 0.0f, 0.0f };
		olc::vf2d dir	= { 0.0f, 0.0f };
		float spd		= 15.0f;
		float size		= 5.0f;
		int fire		= 0;
	};

	struct oPowerUp {
		olc::vf2d pos	= { 0.0f, 0.0f };
		int type		= 0;
		float spd		= 5.0f;
		float size		= 5.0f;
		
	};

	std::vector<oBall> Balls;
	std::vector<oPowerUp> Power;

	std::unique_ptr<int[]> bricks;
	olc::vi2d bricksSize = { 16,16 };

	enum wallType {
		noone,
		breakOne,
		breakTwo,
		breakThree,
		fixedWall,
	};

	enum powerType {
		nothing,
		ballBig,
		ballDouble,
		playerBig,
		playerSmall,
		fireBall,
		fastBall,
		extraBall
	};

public:

	bool OnUserCreate() override
	{
		// Build bricks for level
		bricks = std::make_unique<int[]>(20 * 30);
		for (int _y = 0; _y < 30; _y++)
		{
			for (int _x = 0; _x < 20; _x++)
			{
				if (_x == 0 || _y == 0 || _x == 19 || _y == 29) {
					bricks[_y * 20 + _x] = wallType::fixedWall;
				}
				else {
					bricks[_y * 20 + _x] = wallType::noone;
				};
				if (_x > 2 && _x < 17 && _y > 7 && _y <= 9) {
					bricks[_y * 20 + _x] = wallType::breakOne;
				};
				if (_x > 1 && _x < 18 && _y > 9 && _y <= 10) {
					bricks[_y * 20 + _x] = wallType::breakOne;
				};
				if (_x > 2 && _x < 17 && _y > 2 && _y <= 4) {
					bricks[_y * 20 + _x] = wallType::breakTwo;
				};

				if (_x > 1 && _x < 18 && _y > 4 && _y <= 7) {
					bricks[_y * 20 + _x] = wallType::breakThree;
				};
			}
		};

		// Spawning starting ball
		float spawnAngle = -0.5f;
		Balls.push_back({ {10.0f, 25.0f} , {cos(1.5f),  sin(1.5f)} });

		return true;
	};

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::GREY);
		// Drawing balls
		for (auto& b : Balls) {
			FillCircle(b.pos.x * bricksSize.x, b.pos.y * bricksSize.y, b.size, ((b.fire > 0) ? olc::YELLOW : olc::BLACK));
		};
		
		// Drawing / Collision PowerUps
		int j = 0;
		for (auto& p : Power) {
			p.pos.y += (p.spd) * fElapsedTime;
			switch (p.type)
			{
			case powerType::ballBig:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::DARK_BLUE);
				break;
			case powerType::ballDouble:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::BLUE);
				break;
			case powerType::playerBig:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::GREEN);
				break;
			case powerType::playerSmall:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::YELLOW);
				break;
			case powerType::fireBall:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::RED);
				break;
			case powerType::fastBall:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::MAGENTA);
				break;
			case powerType::extraBall:
				FillRect((p.pos.x * bricksSize.x) - (bricksSize.x / 2), (p.pos.y * bricksSize.y) - (bricksSize.y / 2), bricksSize.x, bricksSize.y, olc::CYAN);
				break;
			default:
				break;
			}

			// Collision with player
			if (p.pos.y >= 25.0f && p.pos.y < 25.1f) { // Y check
				if (p.pos.x >= playerPos - (playerWidth) && p.pos.x <= playerPos + (playerWidth)) { // X check

					//Apply buff/debuff
					switch (p.type)
					{
					case powerType::ballBig:
						for (auto& b : Balls) {
							b.size++;
						}
						break;
					case powerType::ballDouble:
						for (auto& b : Balls) {
							Balls.push_back({ b.pos , {cos(-0.5f), sin(-0.5f)} });
						}
						break;
					case powerType::playerBig:
						playerWidth++;
						break;
					case powerType::playerSmall:
						playerWidth -= 0.2;
						break;
					case powerType::fireBall:
						for (auto& b : Balls) {
							b.fire = 20;
						}
						break;
					case powerType::fastBall:
						for (auto& b : Balls) {
							b.spd++;
						}
						break;
					case powerType::extraBall:
						Balls.push_back({ {playerPos, 25.0f} , {cos(-0.5f), sin(-0.5f)} });
						break;
					default:
						break;
					}

					Power.erase(Power.begin() + j);
				}
			};
			
			j++;
		};

		// Drawing Bricks
		for (int _y = 0; _y < 30; _y++)
		{
			for (int _x = 0; _x < 20; _x++)
			{
				switch (bricks[_y * 20 + _x])
				{
				case wallType::fixedWall:
					FillRect((_x * bricksSize.x), (_y * bricksSize.y), bricksSize.x, bricksSize.y, olc::BLACK);
					break;
				case wallType::breakOne:
					FillRect((_x * bricksSize.x) + 1, (_y * bricksSize.y) + 1, 14, 14, olc::GREEN);
					break;
				case wallType::breakTwo:
					FillRect((_x * bricksSize.x) + 1, (_y * bricksSize.y) + 1, 14, 14, olc::YELLOW);
					break;
				case wallType::breakThree:
					FillRect((_x * bricksSize.x) + 1, (_y * bricksSize.y) + 1, 14, 14, olc::RED);
					break;
				}
			}

		};

		// Moving balls
		int i = 0;
		for (auto& b : Balls) {
			olc::vf2d potentialPos = b.pos + (b.dir * b.spd) * fElapsedTime;
			olc::vf2d ballSize = { b.size / bricksSize.x, b.size / bricksSize.y };

			auto testPotentialPos = [&](const olc::vf2d& dirToCheck) {
				olc::vi2d testPoint = potentialPos + ballSize * dirToCheck;

				auto& tile = bricks[(testPoint.y) * 20 + (testPoint.x)];
				if (tile == wallType::noone)
				{
					return false;
				}
				else
				{
					// Collision
					bool tileHit = tile < wallType::fixedWall;

					// Power ups
					if ((rand() % 10) + 1 < 2 && tile != wallType::fixedWall) {
						int r = (rand() % 7) + 1;
						if (r < 8) {
							Power.push_back({ { b.pos.x, b.pos.y}, r});
						}
					}

					//update tiles hp
					if (tileHit) tile--;

					// If fire ball
					if (b.fire > 0 && tile != wallType::fixedWall) {
						b.fire--;
					} else {
						if (dirToCheck.x == 0.0f) {
							b.dir.y *= -1.0f;
						}
						if (dirToCheck.y == 0.0f) {
							b.dir.x *= -1.0f;
						}
					}

					return tileHit;
				}
			};

			// Calling collision checks
			testPotentialPos(olc::vf2d(0, -1));
			testPotentialPos(olc::vf2d(0, +1));
			testPotentialPos(olc::vf2d(-1, 0));
			testPotentialPos(olc::vf2d(+1, 0));

			// Collision with player
			if (potentialPos.y >= 25.0f && potentialPos.y < 25.1f) { // Y check
				if (potentialPos.x >= playerPos - (playerWidth) && potentialPos.x <= playerPos + (playerWidth)) { // X check
					float newAngle = std::clamp((((b.pos.x + ballSize.x) - (playerPos + playerWidth)) / playerWidth*2), -2.7f, -0.4f);
					b.dir = {cos(newAngle), sin(newAngle) };
					

				}
			};

			// Delete ball
			if (b.pos.y > 26.0f) {
				Balls.erase(Balls.begin() + i);
			};

			//Update pos
			b.pos += b.dir * b.spd * fElapsedTime;

			i++;
		};

		// Drawing Player
		FillRect((playerPos - (playerWidth)) * bricksSize.x, (25.0f) * bricksSize.y, ((playerWidth * 2) * bricksSize.x), (4.0f));

		// Moving player
		if (GetKey(olc::Key::LEFT).bHeld) playerPos -= playerSpeed * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) playerPos += playerSpeed * fElapsedTime;

		playerPos = std::clamp(playerPos, playerWidth, 20.f - playerWidth);
		//End
		return true;
	};
};

int main()
{
	Game game;
	if (game.Construct(320, 460, 1, 1))
		game.Start();
	return 0;
}