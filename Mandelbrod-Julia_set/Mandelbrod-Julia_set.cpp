
#include "pch.h"
#include <iostream>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


struct Vect2
{
	double x, y;

	Vect2 operator-(const Vect2 other) const
	{
		return { x - other.x,y - other.y };
	}

	Vect2 operator+(const Vect2 other) const
	{
		return { x + other.x,y + other.y };
	}

	Vect2 operator*(const Vect2 other) const
	{
		return { x * other.x,y * other.y };
	}

	Vect2 operator*(const float other) const
	{
		return { x * other,y * other };
	}
};

class PixelController : public olc::PixelGameEngine
{


	struct Zoom
	{
		Vect2 point1, point2;
	};

	enum State
	{
		MandelBrod = 0,
		Julia = 1
	};

	olc::Sprite* fractelSprite;
	Vect2 juliaC = { 0,0 };

public:
	PixelController()
	{
		sAppName = "Hello_world";
	}

	int currentZoom = 0;
	std::vector<Zoom> zoomHistory = { {{-2,-2},{2,2}} };

	double scaleX;
	double scaleY;

public:
	bool OnUserCreate() override
	{
		std::cout << "creation" << std::endl;
		scaleX = (zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x) / ScreenWidth();
		scaleY = (zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y) / ScreenHeight();

		fractelSprite = new olc::Sprite(ScreenWidth(), ScreenHeight());
		CalcMandelBrod({ -2,-2 }, { 2,2 }, (*fractelSprite));
		DrawSprite(0, 0,&(*fractelSprite));
		return true;
	}

	
	std::vector<Vect2> JuliaPath = { };
	int juliaPathTraceIndex = 0;
	float totalTraceTime = 0;
	bool doJuliaPathTrace = false;
	State state = MandelBrod;
public:
	bool OnUserUpdate(float fElapsedTime) override
	{

		if(IsFocused())
		{
			ChangeJuliaC();
			ChangeJuliaPath();
			ToggleJuliaPathTrace();
			ZoomIn();
			ZoomOut();
			SwichModes();
		}
		TraceJuliaPath(fElapsedTime);
		
		return true;
	}

	void CalcMandelBrod(Vect2 from, Vect2 to, olc::Sprite &sprite)
	{
		int imgXSize = sprite.width;
		int imgYSize = sprite.height;

		Vect2 stepSize = to - from;
		stepSize.x /= imgXSize;
		stepSize.y /= imgYSize;
		Vect2 coordinate;
		coordinate.x = from.x;
		for(int i = 0; i < imgXSize; i++)
		{

			coordinate.x += stepSize.x;
			coordinate.y = from.y;
			for(int j = 0; j<imgYSize; j++)
			{
				coordinate.y += stepSize.y;
				float a = 0;
				float b = 0;
				int n = 0;
				while(n<100)
				{
					float na = (a * a) - (b * b);
					float nb = 2 * a * b;
					a = na + coordinate.x;
					b = nb + coordinate.y;

					if (a + b > 16)
						break;

					n++;
				}
				//std::cout << n << std::endl;
				sprite.SetPixel(i, j, GenerateColor(n));
			}
		}
	}
	void CalcJulia(Vect2 from, Vect2 to,Vect2 C,olc::Sprite &sprite)
	{
		int imgXSize = sprite.width;
		int imgYSize = sprite.height;

		Vect2 stepSize = to - from;
		stepSize.x /= imgXSize;
		stepSize.y /= imgYSize;
		Vect2 coordinate;
		coordinate.x = from.x;
		for (int i = 0; i < imgXSize; i++)
		{

			coordinate.x += stepSize.x;
			coordinate.y = from.y;
			for (int j = 0; j < imgYSize; j++)
			{
				coordinate.y += stepSize.y;
				float a = coordinate.x;
				float b = coordinate.y;
				int n = 0;
				while (n < 100)
				{
					float na = (a * a) - (b * b);
					float nb = 2 * a * b;
					a = na + C.x;
					b = nb + C.y;

					if (a + b > 16)
					{
						break;
					}

					n++;
				}

				sprite.SetPixel(i, j, GenerateColor(n));
			}
		}
	}
	olc::Pixel GenerateColor(int n)
	{
		n /= 10;
		switch (n)
		{
		default:
			return olc::WHITE;
			break;
		case 1:
			return olc::WHITE;
			break;
		case 2:
			return olc::BLUE;
			break;
		case 3:
			return olc::GREEN;
			break;
		case 4:
			return olc::RED;
			break;
		case 5:
			return olc::DARK_BLUE;
			break;
		case 6:
			return olc::DARK_GREEN;
			break;
		case 7:
			return olc::DARK_RED;
			break;
		case 8:
			return olc::VERY_DARK_BLUE;
			break;
		case 9:
			return olc::VERY_DARK_GREEN;
			break;
		case 10:
			return olc::BLACK;
			break;
		}

	}

	void ZoomIn()
	{
		if (GetMouse(1).bPressed)
		{
			currentZoom++;
			zoomHistory.push_back({ {-2,-2},{2,2} });
			zoomHistory[currentZoom].point1.x = GetMouseX();
			zoomHistory[currentZoom].point1.y = GetMouseY();
		}

		if (GetMouse(1).bHeld)
		{
			zoomHistory[currentZoom].point2.x = GetMouseX();
			zoomHistory[currentZoom].point2.y = GetMouseY();
			DrawSprite(0, 0, fractelSprite);
			DrawRect(zoomHistory[currentZoom].point1.x, zoomHistory[currentZoom].point1.y,
				zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x,
				zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y);

		}

		if (GetMouse(1).bReleased)
		{
			zoomHistory[currentZoom].point1.x = scaleX * zoomHistory[currentZoom].point1.x + zoomHistory[currentZoom - 1].point1.x;
			zoomHistory[currentZoom].point2.x = scaleX * zoomHistory[currentZoom].point2.x + zoomHistory[currentZoom - 1].point1.x;
			zoomHistory[currentZoom].point1.y = scaleY * zoomHistory[currentZoom].point1.y + zoomHistory[currentZoom - 1].point1.y;
			zoomHistory[currentZoom].point2.y = scaleY * zoomHistory[currentZoom].point2.y + zoomHistory[currentZoom - 1].point1.y;

			scaleX = (zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x) / ScreenWidth();
			scaleY = (zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y) / ScreenHeight();

			CalcMandelBrod(zoomHistory[currentZoom].point1, zoomHistory[currentZoom].point2, (*fractelSprite));
			DrawSprite(0, 0, fractelSprite);
		}
	}
	void ZoomOut()
	{

		if (GetKey(olc::D).bPressed && currentZoom > 0)
		{
			zoomHistory.erase(zoomHistory.begin() + currentZoom);
			currentZoom--;

			scaleX = (zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x) / ScreenWidth();
			scaleY = (zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y) / ScreenHeight();

			CalcMandelBrod(zoomHistory[currentZoom].point1, zoomHistory[currentZoom].point2, (*fractelSprite));
			DrawSprite(0, 0, fractelSprite);
			
		}
	}
	void SwichModes()
	{
		if (GetKey(olc::S).bPressed)
		{
			if (state == MandelBrod)
			{
				state = Julia;
				CalcJulia(zoomHistory[currentZoom].point1, zoomHistory[currentZoom].point2, juliaC, (*fractelSprite));
				DrawSprite(0, 0, fractelSprite);
				UpdateJulia();
			}
			else
			{
				state = MandelBrod;
				CalcMandelBrod(zoomHistory[currentZoom].point1, zoomHistory[currentZoom].point2, (*fractelSprite));
				DrawSprite(0, 0, fractelSprite);
			}
		}
	}
	void ChangeJuliaC()
	{
		if (state == Julia)
		{
			if (GetMouse(0).bPressed && !GetKey(olc::CTRL).bHeld)
			{

				scaleX = (zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x) / ScreenWidth();
				scaleY = (zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y) / ScreenHeight();

				juliaC.x = GetMouseX()*scaleX + zoomHistory[currentZoom].point1.x;
				juliaC.y = GetMouseY()*scaleY + zoomHistory[currentZoom].point1.y;

				CalcJulia(zoomHistory[currentZoom].point1, zoomHistory[currentZoom].point2, juliaC, (*fractelSprite));
				DrawSprite(0, 0, fractelSprite);
				UpdateJulia();
			}
		}
	}

	void ChangeJuliaPath()
	{
		if(GetKey(olc::CTRL).bHeld)
		{
			if(GetMouse(0).bPressed)
			{
				double scaleX = (zoomHistory[currentZoom].point2.x - zoomHistory[currentZoom].point1.x) / ScreenWidth();
				double scaleY = (zoomHistory[currentZoom].point2.y - zoomHistory[currentZoom].point1.y) / ScreenHeight();
				JuliaPath.push_back(
					{ scaleX*GetMouseX() + zoomHistory[currentZoom].point1.x,
					scaleY*GetMouseY() + zoomHistory[currentZoom].point1.y });
				UpdateJulia();
			}

			if(GetKey(olc::D).bPressed)
			{
				JuliaPath.erase(JuliaPath.end());
			}
		}
	}
	
	void UpdateJulia()
	{
		DrawSprite(0, 0, fractelSprite);
		FillCircle((juliaC.x - zoomHistory[currentZoom].point1.x) / scaleX,
			(juliaC.y - zoomHistory[currentZoom].point1.y) / scaleY, 10, olc::RED);

		for(auto point:JuliaPath)
		{
			FillCircle((point.x - zoomHistory[currentZoom].point1.x) / scaleX,
				(point.y - zoomHistory[currentZoom].point1.y) / scaleY, 5, olc::BLUE);
		}

		for (int i = 1; i < JuliaPath.size(); i++)
		{
			DrawLine((JuliaPath[i - 1].x - zoomHistory[currentZoom].point1.x) / scaleX,
				(JuliaPath[i - 1].y - zoomHistory[currentZoom].point1.y) / scaleY,
				(JuliaPath[i].x - zoomHistory[currentZoom].point1.x) / scaleX,
				(JuliaPath[i].y - zoomHistory[currentZoom].point1.y) / scaleY,
				olc::BLUE);
		}
		

	}

	void TraceJuliaPath(float fElapsedTime)
	{
		if(doJuliaPathTrace)
		{
			std::cout << (JuliaPath[juliaPathTraceIndex] *totalTraceTime*0.1).x << std::endl;
			juliaC = JuliaPath[juliaPathTraceIndex] +
				(JuliaPath[juliaPathTraceIndex + 1] - JuliaPath[juliaPathTraceIndex] * totalTraceTime*0.1);
			CalcJulia(zoomHistory[currentZoom].point1,zoomHistory[currentZoom].point2,juliaC,(*fractelSprite));
			UpdateJulia();
			totalTraceTime += fElapsedTime;
			
			if (totalTraceTime >= 8)
			{
				juliaPathTraceIndex += 1;
				std::cout << (float)juliaC.x << std::endl;
				totalTraceTime = 0;
				std::cout << "update index" << std::endl;
			}

			if(juliaPathTraceIndex >= JuliaPath.size()-1)
			{
				std::cout << "endpath" << std::endl;
				doJuliaPathTrace = false;
			}
		}

	}

 	void ToggleJuliaPathTrace()
	{
		if (GetKey(olc::P).bPressed && !doJuliaPathTrace)
		{
			doJuliaPathTrace = true;
			std::cout << "startpath" << std::endl;
		}

		else if (GetKey(olc::P).bPressed && doJuliaPathTrace)
		{
			std::cout << "shutpath" << std::endl;
			doJuliaPathTrace = false;
		}
	}

};

int main()
{
	PixelController controller;
	if (controller.Construct(800, 800, 1, 1))
		controller.Start();
}