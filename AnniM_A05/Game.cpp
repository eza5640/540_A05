#include "Game.h"
#include "Vertex.h"
#include <iostream>
#include <filesystem>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vBuff = 0;
	vBuff2 = 0;
	vBuff3 = 0;
	vertexShader = 0;
	pixelShader = 0;
	firstMesh = 0;
	secondMesh = 0;
	thirdMesh = 0;
	entity = 0;
	entity2 = 0;
	entity3 = 0;
	entity4 = 0;
	entity5 = 0;
	cam = 0;
	material = 0;
	coneMesh = 0;
	prevMousePos = { 0,0 };

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vBuff) { vBuff->Release(); }
	if (vBuff2) { vBuff2->Release(); }
	if (vBuff3) { vBuff3->Release(); }

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete firstMesh;
	delete secondMesh;
	delete thirdMesh;
	delete entity;
	delete entity2;
	delete entity3;
	delete entity4;
	delete entity5;
	delete material;
	delete cam;
	delete coneMesh;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!
	

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	cam = new Camera();
	XMVECTOR pos = XMVectorSet(0, 0, -25, 0);
	XMVECTOR direction = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		direction,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!
	//cam->SetView(V);

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
	cam->SetProj(width, height);

}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};
	Vertex vertices2[] =
	{
		{ XMFLOAT3(+2.0f, +5.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+1.5f, +4.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.5f, +4.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};
	Vertex vertices3[] =
	{
		{ XMFLOAT3(-5.0f, +5.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-4.5f, +3.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-5.5f, +3.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};
	//};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	int indices[] = { 0, 1, 2 };
	int indices2[] = { 0, 1, 2 };
	int indices3[] = { 0, 1, 2 };


	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	//D3D11_BUFFER_DESC vbd;
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = sizeof(Vertex) * 3;       // 3 = number of vertices in the buffer
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	//vbd.CPUAccessFlags = 0;
	//vbd.MiscFlags = 0;
	//vbd.StructureByteStride = 0;

	//// Create the proper struct to hold the initial vertex data
	//// - This is how we put the initial data into the buffer
	//D3D11_SUBRESOURCE_DATA initialVertexData;
	//initialVertexData.pSysMem = vertices;

	//// Actually create the buffer with the initial data
	//// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	//device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);



	//// Create the INDEX BUFFER description ------------------------------------
	//// - The description is created on the stack because we only need
	////    it to create the buffer.  The description is then useless.
	//D3D11_BUFFER_DESC ibd;
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.ByteWidth = sizeof(int) * 3;         // 3 = number of indices in the buffer
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	//ibd.CPUAccessFlags = 0;
	//ibd.MiscFlags = 0;
	//ibd.StructureByteStride = 0;

	//// Create the proper struct to hold the initial index data
	//// - This is how we put the initial data into the buffer
	//D3D11_SUBRESOURCE_DATA initialIndexData;
	//initialIndexData.pSysMem = indices;

	//// Actually create the buffer with the initial data
	//// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	//device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
	coneMesh = new Mesh("cone.obj", device);
	//firstMesh = new Mesh(vertices, (int)sizeof(vertices), (unsigned int*)(&indices), (int)sizeof(indices), device);
	material = new Material(vertexShader, pixelShader);
	//secondMesh = new Mesh(vertices2, (int)sizeof(vertices2), (unsigned int*)(&indices2), (int)sizeof(indices2), device);
	//thirdMesh = new Mesh(vertices3, (int)sizeof(vertices3), (unsigned int*)(&indices3), (int)sizeof(indices3), device);
	entity = new GameEntity(coneMesh, material);
	entity->SetWorld(XMLoadFloat4x4(&worldMatrix));
	/*entity2 = new GameEntity(firstMesh);
	entity->SetWorld(XMLoadFloat4x4(&worldMatrix));
	entity3 = new GameEntity(firstMesh);
	entity->SetWorld(XMLoadFloat4x4(&worldMatrix));
	entity4 = new GameEntity(secondMesh);
	entity->SetWorld(XMLoadFloat4x4(&worldMatrix));
	entity5 = new GameEntity(thirdMesh);
	entity->SetWorld(XMLoadFloat4x4(&worldMatrix));*/
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!

	cam->SetProj(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//entity->Move(XMFLOAT3(10.f * deltaTime, 0, 0));
	//entity2->Move(XMFLOAT3(0, 10.f * deltaTime, 0));
	//entity3->Move(XMFLOAT3(0, 0, 10.f * deltaTime));
	//entity4->Rotate(XMFLOAT3(0, 0, 0.5f * deltaTime));
	//entity5->Scale(XMFLOAT3(0.2f * deltaTime, 0, 0.002f * deltaTime));

	cam->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	entity->PrepareMaterial(cam->GetView(), cam->GetProj());
	//vertexShader->SetMatrix4x4("world", entity->GetWorld());
	//vertexShader->SetMatrix4x4("view", cam->GetView());
	//vertexShader->SetMatrix4x4("projection", projectionMatrix);

	//// Once you've set all of the data you care to change for
	//// the next draw call, you need to actually send it to the GPU
	////  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	//vertexShader->CopyAllBufferData();

	//// Set the vertex and pixel shaders to use for the next Draw() command
	////  - These don't technically need to be set every frame...YET
	////  - Once you start applying different shaders to different objects,
	////    you'll need to swap the current shaders before each draw
	//vertexShader->SetShader();
	//pixelShader->SetShader();

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	vBuff = entity->gameMesh->GetVertexBuffer();

	context->IASetVertexBuffers(0, 1, &vBuff, &stride, &offset);
	context->IASetIndexBuffer(entity->gameMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(
		3,     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	//entity 2
	//vertexShader->SetMatrix4x4("world", entity2->GetWorld());
	//vertexShader->SetMatrix4x4("view", viewMatrix);
	//vertexShader->SetMatrix4x4("projection", projectionMatrix);

	//vBuff = entity2->gameMesh->GetVertexBuffer();
	//context->IASetVertexBuffers(0, 1, &vBuff, &stride, &offset);
	//context->IASetIndexBuffer(entity2->gameMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//vertexShader->CopyAllBufferData();
	//context->DrawIndexed(
	//	3,     // The number of indices to use (we could draw a subset if we wanted)
	//	0,     // Offset to the first index we want to use
	//	0);    // Offset to add to each index when looking up vertices

	//// Once you've set all of the data you care to change for
	//// the next draw call, you need to actually send it to the GPU
	////  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!

	////entity 3
	//vertexShader->SetMatrix4x4("world", entity3->GetWorld());
	//vertexShader->SetMatrix4x4("view", viewMatrix);
	//vertexShader->SetMatrix4x4("projection", projectionMatrix);

	//vBuff = entity3->gameMesh->GetVertexBuffer();
	//context->IASetVertexBuffers(0, 1, &vBuff, &stride, &offset);
	//context->IASetIndexBuffer(entity3->gameMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//vertexShader->CopyAllBufferData();
	//context->DrawIndexed(
	//	3,     // The number of indices to use (we could draw a subset if we wanted)
	//	0,     // Offset to the first index we want to use
	//	0);    // Offset to add to each index when looking up vertices

	//// Once you've set all of the data you care to change for
	//// the next draw call, you need to actually send it to the GPU
	////  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!


	////entity 4
	//vertexShader->SetMatrix4x4("world", entity4->GetWorld());
	//vertexShader->SetMatrix4x4("view", viewMatrix);
	//vertexShader->SetMatrix4x4("projection", projectionMatrix);

	//vBuff2 = entity4->gameMesh->GetVertexBuffer();
	//context->IASetVertexBuffers(0, 1, &vBuff2, &stride, &offset);
	//context->IASetIndexBuffer(entity4->gameMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//vertexShader->CopyAllBufferData();
	//context->DrawIndexed(
	//	3,     // The number of indices to use (we could draw a subset if we wanted)
	//	0,     // Offset to the first index we want to use
	//	0);    // Offset to add to each index when looking up vertices

	////entity 5
	//vertexShader->SetMatrix4x4("world", entity5->GetWorld());
	//vertexShader->SetMatrix4x4("view", viewMatrix);
	//vertexShader->SetMatrix4x4("projection", projectionMatrix);
	//vBuff3 = entity5->gameMesh->GetVertexBuffer();
	//context->IASetVertexBuffers(0, 1, &vBuff3, &stride, &offset);
	//context->IASetIndexBuffer(entity5->gameMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//vertexShader->CopyAllBufferData();
	//context->DrawIndexed(
	//	3,     // The number of indices to use (we could draw a subset if we wanted)
	//	0,     // Offset to the first index we want to use
	//	0);    // Offset to add to each index when looking up vertices


	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER



	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain effect,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (buttonState && 0x0001)
	{
		cam->RotateCam(x - prevMousePos.x, y - prevMousePos.y);
	}
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion