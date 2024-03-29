#include "pch.h"

#include <gfx/GContext.h>
#include <gfx/GWindow.h>
#include <gfx/GBuffer.h>
#include <gfx/GTexture.h>
#include <gfx/GDescHeap.h>
#include <gfx/GShader.h>
#include <gfx/GRootSignature.h>

#include <rendering/RenderState.h>
#include <rendering/Vertex.h>
#include <rendering/QuadManger.h>
#include <rendering/EasyTex.h>

#include <audio/AudioClient.h>
#include <audio/AudioVoice.h>

#include <tools/StopWatch.h>
#include <tools/log.h>
#include <game/Game.h>

INT WINAPI wWinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, LPWSTR _In_ cmdArgs, INT _In_ nCmdShow) {
	Logger logger;
	SetLogger(logger);
	
	// Debug
#ifdef _DEBUG
	ScopedComPointer<ID3D12Debug> ptrDebug;
	D3D12GetDebugInterface(IID_PPV_ARGS(ptrDebug.to()));
	ptrDebug->EnableDebugLayer();
#endif

	// Co init
	HRESULT hr;
	if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		GetLogger().log("CoInitializeEx(...) failed!").log(hr);
		return -1;
	}
	
	// Set random seed
	std::srand(std::time(nullptr));

	// Context
	GContext ctx;
	
	// Window
	EasyHWND::WindowClass cls(L"ShipBattle_WND", CS_OWNDC, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW));
	GWindow wnd(cls, ctx.getDevice(), ctx.getQueue());
	wnd.setWindowVisibility(true);

	// Maximize
	ShowWindow((HWND)wnd, SW_MAXIMIZE);
	wnd.setFullscreen(true);

	// Upload buffer
	GBuffer<BYTE, 1024 * 1024 * 32> uploadBuffer(ctx.getDevice(), D3D12_HEAP_TYPE_UPLOAD);

	// Textures
	GTexture tilemap(L"./res/tilemap.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture backgroundPlay(L"./res/screen_play.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture backgroundStart(L"./res/screen_start.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);

	GTexture statPlaceFleet(L"./res/stat_place_fleet.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statPlaceShot(L"./res/stat_place_shot.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statPlaceShotPC(L"./res/stat_wait_enemy.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statFireShot(L"./res/stat_shot_traveling.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statShotHit(L"./res/stat_hit.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statShotNoHit(L"./res/stat_nohit.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statGameWin(L"./res/stat_game_win.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);
	GTexture statGameLost(L"./res/stat_game_lost.png", ctx, D3D12_HEAP_TYPE_DEFAULT, uploadBuffer);

	// Create heap and views
	GDescHeap textureDHeap(ctx.getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 11);
	auto handleTex = textureDHeap.getCpuHandle();

	// Create views
	tilemap.createSrv(ctx.getDevice(), handleTex.getIncrement());
	backgroundPlay.createSrv(ctx.getDevice(), handleTex.getIncrement());
	backgroundStart.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statPlaceFleet.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statPlaceShot.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statPlaceShotPC.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statFireShot.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statShotHit.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statShotNoHit.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statGameWin.createSrv(ctx.getDevice(), handleTex.getIncrement());
	statGameLost.createSrv(ctx.getDevice(), handleTex.getIncrement());

	// Load shaders
	GShader rootSignatureShader(L"./shader/RootSignature.cso");
	GShader vertexShader(L"./shader/ImageQuadVS.cso");
	GShader pixelShader(L"./shader/ImageQuadPS.cso");

	// Create root signature and pso
	GRootSignature rootSignature(ctx.getDevice(), rootSignatureShader.ptr(), rootSignatureShader.size());
	RenderState renderState(ctx.getDevice(), rootSignature.getSignature(), vertexShader, pixelShader);

	// Quad manager
	QuadManger vertexQuadManager(ctx.getDevice());

	// Timer
	StopWatch timer;
	timer.start();

	// Audio client
	AudioClient audioClient;
	audioClient.setVolume(0.8f);

	// Background track
	AudioVoice bgTrack(audioClient.getAudio(), L"./res/tr0.bad");

	// SFX Tracks
	AudioVoice sfxAimShot(audioClient.getAudio(), L"./res/tr1.bad");
	AudioVoice sfxHit(audioClient.getAudio(), L"./res/tr2.bad");
	AudioVoice sfxNoHit(audioClient.getAudio(), L"./res/tr3.bad");
	AudioVoice sfxPlayerLost(audioClient.getAudio(), L"./res/tr4.bad");
	AudioVoice sfxPlayerWin(audioClient.getAudio(), L"./res/tr5.bad");

	// SFX Array
	AudioVoice* sfxArray[] = {
		&sfxAimShot,
		&sfxHit,
		&sfxNoHit,
		&sfxPlayerLost,
		&sfxPlayerWin,
		&bgTrack
	};

	// Game
	Game game(sfxArray);

	// Play BG Track
	bgTrack.play(true);

	// Loop
	MSG msg = {};
	while (!wnd.checkWindowCloseFlag()) {
		// Resize
		if (wnd.needsResizing())
		{
			ctx.flush();
			wnd.resize(ctx.getDevice());
		}
		

		// Event loop
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Gfx loop
		ID3D12GraphicsCommandList* ptrCmdList = ctx.getList();

		// Begin frame
		wnd.beginFrame(ptrCmdList);

		// Bind core stuff
		ptrCmdList->SetGraphicsRootSignature(rootSignature.getSignature());
		ptrCmdList->SetPipelineState(renderState.getState());
		ID3D12DescriptorHeap* heaps[] = {
			textureDHeap.getHeap()
		};
		ptrCmdList->SetDescriptorHeaps(1, heaps);
		ptrCmdList->SetGraphicsRootDescriptorTable(0, textureDHeap.getGpuHandle());

		// Viewport and rect
		const D3D12_VIEWPORT vp = {0,0,wnd.getWidth(),wnd.getHeight(),0.0f,1.0f};
		const D3D12_RECT sr = { 0,0,wnd.getWidth(),wnd.getHeight() };
		ptrCmdList->RSSetViewports(1, &vp);
		ptrCmdList->RSSetScissorRects(1, &sr);

		// Get Delta time reset and call game logic
		auto deltaTMs = timer.deltaMs();
		timer.reset();
		game.update(deltaTMs, vertexQuadManager, wnd.getCurrentIOState());

		// Draw
		auto vertexCount = vertexQuadManager.uploadReset(ptrCmdList);
		if (vertexCount) {
			ptrCmdList->DrawInstanced(vertexCount, 1, 0, 0);
		}

		// End frame and render
		wnd.endFrame(ptrCmdList);
		ctx.execute();
		if (!wnd.present()) {
			GetLogger().log("Frame presentation failed!");
			return -1;
		}
	}

	// Background track stop
	bgTrack.stop();

	// FLush
	ctx.flush();

	// Uninit
	CoUninitialize();

	// Pass
	return 0;
}
