#include "commonUtil.h"
#include "geoStructs.h"
#include "decFunctions.h"

float timef, timeDelta;
mesh hand;

void updateGeometryOld(std::vector<mesh>& scene)
{
	scene.clear();
	mesh plane;
	plane.tris =
	{
		{1, 0, 1,		1, 0, -1,		-1, 0, 1},
		{-1, 0, 1,		1, 0, -1,		-1, 0, -1}
	};

	// Init cube
	mesh cube;
	cube.tris =
	{
		// TOP
		{-1, 1, -1,		-1, 1, 1,		1, 1, 1},
		{-1, 1, -1,		1, 1, 1,		1, 1, -1},

		// BOTTOM
		{1, -1, 1,		-1, -1, 1,		-1, -1, -1},
		{1, -1, 1,		-1, -1, -1,		1, -1, -1},

		// NORTH
		{1, -1, 1,		1, 1, -1,		1, 1, 1},
		{1, -1, 1,		1, -1, -1,		1, 1, -1},

		// SOUTH
		{-1, 1, 1,		-1, 1, -1,		-1, -1, 1},
		{-1, -1, 1,		-1, 1, -1,		-1, -1, -1},

		// EAST
		{-1, 1, -1,		1, 1, -1,		-1, -1, -1},
		{-1, -1, -1,	1, 1, -1,		1, -1, -1},

		// WEST
		{1, 1, 1,		-1, 1, 1,		-1, -1, 1},
		{1, 1, 1,		-1, -1, 1,		1, -1, 1}
	};

	for (auto& tri : cube.tris) for (auto& pt : tri.pts)
	{
		transform(pt,
			timef, timef * 0.3, 0,
			0.4, 0.4, 0.4,
			0, 0, 0);
	}

	// Create 2D grid of points
	points copyPoints;
	for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++) copyPoints.pts.push_back({ (float)i - 5, (float)j - 5, 0 });

	// Create new cube mesh at each point
	for (vec3d& point : copyPoints.pts)
	{
		scale(point, 2, 2, 2);
		translate(point, 0, 0, std::cosf(timef * 1.5f + point.y * 0.2f));
		mesh geoToCopy = cube;
		for (triangle& tri : geoToCopy.tris) for (vec3d& pos : tri.pts)
		{
			pos = pos + point;
		}

		scene.push_back(geoToCopy);
	}
}

void initGeometry()
{
	hand.loadFromObjFile("hand.obj");
	for (auto& tri : hand.tris) for (auto& pt : tri.pts) scale(pt, 5, 5, 5);
}

void updateGeometry(std::vector<mesh>& scene)
{
	scene.clear();
	mesh handCopy = hand;

	for (auto& tri : handCopy.tris) for (auto& pt : tri.pts)
	{
		scale(pt, 0.5, 0.5, 0.5);
		rotate(pt, 0, timef, 0);
	}

	// Create 2D grid of points
	points copyPoints;
	for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) copyPoints.pts.push_back({ (float)i - 0.5f, (float)j - 0.5f, 0 });

	// Create new cube mesh at each point
	for (vec3d& point : copyPoints.pts)
	{
		scale(point, 4, 4, 4);
		translate(point, 0, 0, std::cosf(timef * 3 + point.y * 0.2f));
		mesh geoToCopy = handCopy;
		for (triangle& tri : geoToCopy.tris) for (vec3d& pos : tri.pts)
		{
			pos = pos + point;
		}

		scene.push_back(geoToCopy);
	}

}

int main()
{
	// Define the sceensize
	float screenHeight = (float)sf::VideoMode::getDesktopMode().height;
	float screenWidth = (float)sf::VideoMode::getDesktopMode().width;

	// Create a window and render space with a title and close button
	sf::RenderWindow window(
		sf::VideoMode((int)screenWidth - 150,
			(int)screenHeight - 300),
		"Game Window", sf::Style::Close);

	// Get the size of the window;
	float windowWidth = (float)window.getSize().x;
	float windowHeight = (float)window.getSize().y;

	// Declare clocks
	sf::Clock clock;
	sf::Clock tick;

	// Define camera
	vec3d cam = { 0, 0, 0 };

	// Define Projection Matrix
	float near = 0.1f;
	float far = 1000;
	float fov = 90;
	float aspectRatio = windowHeight / windowWidth;
	float scalingFactor = 1 / tanf(fov * 0.5f / 180 * 3.14159f);
	matrix4d matProj = {
		{ { aspectRatio * scalingFactor, 0.0f, 0.0f, 0.0f },
		{ 0.0f, scalingFactor, 0.0, 0.0 },
		{ 0.0f, 0.0f, far / (far - near), near * far / (far - near) },
		{ 0.0f, 0.0f, 1.0f, 0.0f} }
	};

	// Declare scene
	std::vector<mesh> scene;
	initGeometry();

	// Start open loop
	while (window.isOpen())
	{
		// Evaluate time
		timef = clock.getElapsedTime().asSeconds();
		timeDelta = tick.restart().asSeconds();

		// If closed, close window
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// Clear screen
		window.clear();

		int id = 0;

		updateGeometry(scene);
		std::vector<triangle> drawTris;

		// World transform and get visible tris
		for (auto& mesh : scene) for (auto& tri : mesh.tris)
		{
			// Create render tri so we don't edit the original tri
			triangle renderTri;
			renderTri = tri;
			int seed;
			seed = id;

			// Do world transform
			for (auto& pt : renderTri.pts)
			{
				//rotate(pt, timef * 0.3, timef * 0.4, timef * 0.5);
				rotate(pt, 0, timef * 0.2, 0);
				pt.z += 5;
			}

			// Calculate Normal in Camera Space
			vec3d N, vec1, vec2;
			vec1 = renderTri.pts[1] - renderTri.pts[0];
			vec2 = renderTri.pts[2] - renderTri.pts[0];
			N = cross(vec1, vec2);
			N = normalize(N);

			vec3d camRay = renderTri.pts[0] - cam;

			// Cull backfaces
			if (dot(camRay, N) < 0.0)
			{
				// Do projection from 3D to 2D
				for (auto& pt : renderTri.pts) project(pt, matProj);

				// Generate random colors
				std::mt19937 rRand(seed);
				std::mt19937 gRand(rRand());
				std::mt19937 bRand(gRand());
				std::uniform_int_distribution<int> dis(0, 255);
				int r = dis(rRand);
				int g = dis(gRand);
				int b = dis(bRand);

				vec3d colorRGB = { (float)r, (float)g, (float)b };
				vec3d colorHSV = rgbToHsv(colorRGB);
				colorHSV = { colorHSV.x, colorHSV.y * 0.5f , colorHSV.z * 0.5f + 0.5f };
				colorRGB = hsvToRgb(colorHSV);

				vec3d lightDir = { 0,0,-1 };
				float lightDot = std::fmax(dot(N, lightDir), 0);

				colorRGB = { colorRGB.x * lightDot, colorRGB.y * lightDot, colorRGB.z * lightDot };
				renderTri.color = colorRGB;

				drawTris.push_back(renderTri);
			}
			id++;
		}

		// Sort triangles by depth
		sort(drawTris.begin(), drawTris.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.pts[0].z + t1.pts[1].z + t1.pts[2].z) / 3.0f;
				float z2 = (t2.pts[0].z + t2.pts[1].z + t2.pts[2].z) / 3.0f;
				return z1 < z2;
			});

		for (auto& tri : drawTris)
		{
			// Create sfml triangle
			sf::VertexArray triDraw(sf::TriangleStrip, 4);
			triDraw[0].position = sf::Vector2f((tri.pts[0].x * 0.5f + 0.5f) * windowWidth, (tri.pts[0].y * 0.5f + 0.5f) * windowHeight);
			triDraw[1].position = sf::Vector2f((tri.pts[1].x * 0.5f + 0.5f) * windowWidth, (tri.pts[1].y * 0.5f + 0.5f) * windowHeight);
			triDraw[2].position = sf::Vector2f((tri.pts[2].x * 0.5f + 0.5f) * windowWidth, (tri.pts[2].y * 0.5f + 0.5f) * windowHeight);
			triDraw[3].position = sf::Vector2f((tri.pts[0].x * 0.5f + 0.5f) * windowWidth, (tri.pts[0].y * 0.5f + 0.5f) * windowHeight);

			sf::Color color;

			color = sf::Color((int)tri.color.x, (int)tri.color.y, (int)tri.color.z);

			triDraw[0].color = color;
			triDraw[1].color = color;
			triDraw[2].color = color;
			triDraw[3].color = color;

			// Draw to screen
			window.draw(triDraw);
		}
		window.display();
	}
}