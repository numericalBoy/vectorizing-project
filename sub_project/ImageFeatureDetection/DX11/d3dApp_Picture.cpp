#include "stdafx.h"
#include "d3dApp_Picture.h"
#include <d3d11.h>
#include <d3dx10.h>

D3DApp_Picture::D3DApp_Picture()
{
	m_Scale = 1;
	m_LookCenterX = 0;
	m_LookCenterY = 0;
	m_ClientWidth = 0;
	m_ClientHeight = 0;
	m_Pics_Texture = 0;
	m_PicW = 0;
	m_PicH = 0;
	m_MouseType = NONE_LINE;
	m_d3dDevice = NULL;
	m_SwapChain = NULL;
	m_Pics_Texture = NULL;
	m_Pics_Buffer = NULL;
	m_Pics_Effect = NULL;
	m_Pics_PTech = NULL;
	m_Pics_PLayout = NULL;
	m_Pics_Width = NULL;
	m_Pics_Height = NULL;
	m_Pics_Scale = NULL;
	m_Pics_CenterX = NULL;
	m_Pics_CenterY = NULL;
	m_Points_Buffer = NULL;
	m_Points_Effect = NULL;
	m_Points_PTech = NULL;
	m_Points_PLayout = NULL;
	m_Points_Width = NULL;
	m_Points_Height = NULL;
	m_Points_Scale = NULL;
	m_Points_CenterX = NULL;
	m_Points_CenterY = NULL;
	m_Points_Transparency = NULL;
	m_CircleLine_Buffer = NULL;
	m_CircleLine_Effect = NULL;
	m_CircleLine_PTech = NULL;
	m_CircleLine_PLayout = NULL;
	m_CircleLine_Width = NULL;
	m_CircleLine_Height = NULL;
	m_CircleLine_Scale = NULL;
	m_CircleLine_CenterX = NULL;
	m_CircleLine_CenterY = NULL;
	m_CircleLine_Transparency = NULL;
	m_SkeletonLines_Buffer = NULL;
	m_SkeletonLines_Effect = NULL;
	m_SkeletonLines_PTech = NULL;
	m_SkeletonLines_PLayout = NULL;
	m_SkeletonLines_Width = NULL;
	m_SkeletonLines_Height = NULL;
	m_SkeletonLines_Scale = NULL;
	m_SkeletonLines_CenterX = NULL;
	m_SkeletonLines_CenterY = NULL;
	m_SkeletonLines_Transparency = NULL;
	m_DepthStencilBuffer = NULL;
	m_DepthStencilView2 = NULL;
	m_RenderTargetView = NULL;
	m_DepthStencilView = NULL;
	m_DeviceContext = NULL;
	m_Pics_Width = NULL;
	m_Pics_Height = NULL;
	m_hAppInst   = GetModuleHandle(NULL);
	m_AppPaused  = false;
	m_Minimized  = false;
	m_Maximized  = false;
	m_Resizing   = false;
	m_FrameStats = L"";
	//mFont               = 0;
	m_MainWndCaption = L"D3D11 Application";
	m_d3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
	//md3dDriverType  = D3D_DRIVER_TYPE_REFERENCE;
	m_ClearColor     = D3DXCOLOR(0, 0, 0, 1.0f);
	m_ClientWidth    = 1440;
	m_ClientHeight   = 900;
}

D3DApp_Picture::~D3DApp_Picture()
{
	ReleaseCOM(m_DepthStencilBuffer);
	ReleaseCOM(m_DepthStencilView2);
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_Pics_Buffer);
	ReleaseCOM(m_Points_Buffer);
	ReleaseCOM(m_CircleLine_Buffer);
	ReleaseCOM(m_SkeletonLines_Buffer);
	ReleaseCOM(m_Pics_Effect);
	ReleaseCOM(m_Points_Effect);
	ReleaseCOM(m_CircleLine_Effect);
	ReleaseCOM(m_SkeletonLines_Effect);
	ReleaseCOM(m_Pics_PLayout);
	ReleaseCOM(m_Points_PLayout);
	ReleaseCOM(m_CircleLine_PLayout);
	ReleaseCOM(m_SkeletonLines_PLayout);
	ReleaseCOM(m_pBlendState_BLEND);
	ReleaseCOM(m_pBlendState_ADD);
	ReleaseCOM(m_pDepthStencil_ZWriteON);
	ReleaseCOM(m_pDepthStencil_ZWriteOFF);
	ReleaseCOM(m_Backup_Buffer);
	ReleaseCOM(m_DeviceContext);
	ReleaseCOM(m_d3dDevice);
	ReleaseCOM(m_SwapChain);
}

HINSTANCE D3DApp_Picture::getAppInst()
{
	return m_hAppInst;
}

HWND D3DApp_Picture::getMainWnd()
{
	return m_hMainWnd;
}

void D3DApp_Picture::initApp(HWND hWnd, int w, int h)
{
	m_hMainWnd = hWnd;
	m_ClientWidth = w;
	m_ClientHeight = h;
	if (m_ClientWidth == 0)
	{
		m_ClientWidth = 100;
		m_ClientHeight = 100;
	}
	initDirect3D();
	LoadBlend();
}

void D3DApp_Picture::initDirect3D()
{
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// No multisampling.
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = m_hMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
	// Create the device.
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT               numLevelsRequested = 1;
	HR(D3D11CreateDeviceAndSwapChain(
		   0,                 //default adapter
		   m_d3dDriverType,
		   0,                 // no software device
		   createDeviceFlags,
		   &FeatureLevelsRequested,
		   numLevelsRequested,
		   D3D11_SDK_VERSION,
		   &sd,
		   &m_SwapChain,
		   &m_d3dDevice,
		   &m_FeatureLevelsSupported,
		   &m_DeviceContext));
	OnResize(m_ClientWidth, m_ClientHeight);
	m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vbd.CPUAccessFlags = 0;
	m_vbd.MiscFlags = 0;
}


void D3DApp_Picture::OnResize(int w, int h)
{
	if (!m_d3dDevice)
	{
		return;
	}
	m_ClientWidth = w;
	m_ClientHeight = h;
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);
	// Resize the swap chain and recreate the render target view.
	HR(m_SwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight,
								  DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
							  reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
	ReleaseCOM(backBuffer);
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = m_ClientWidth;
	depthStencilDesc.Height    = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags      = 0;
	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0,
										   &m_DepthStencilView));
	// Bind the render target view and depth/stencil view to the pipeline.
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	// Set the viewport transform.
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float)m_ClientWidth;
	vp.Height   = (float)m_ClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports(1, &vp);
	InterSetSize(m_ClientWidth, m_ClientHeight);
}

void D3DApp_Picture::DrawScene()
{
	if (!m_DeviceContext)
	{
		return;
	}
	InterSetSize(m_ClientWidth, m_ClientHeight);
	InterSetLookCenter(m_LookCenterX, m_LookCenterY);
	InterSetScale(m_Scale);
	InterDraw();
	m_SwapChain->Present(0, 0);
}

void D3DApp_Picture::BuildShaderFX()
{
	ID3D10Blob* pCode;
	ID3D10Blob* pError;
	HRESULT hr = 0;
	HR(D3DX11CompileFromFile(_T("shader\\picture.fx"), NULL, NULL, NULL,
							 "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
							 &pCode, &pError, NULL));
	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
									pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Pics_Effect));
	pCode->Release();
	m_Pics_PTech = m_Pics_Effect->GetTechniqueByName("PointTech");
	m_Pics_Width = m_Pics_Effect->GetVariableByName("width")->AsScalar();
	m_Pics_Height = m_Pics_Effect->GetVariableByName("height")->AsScalar();
	m_Pics_CenterX = m_Pics_Effect->GetVariableByName("centerX")->AsScalar();
	m_Pics_CenterY = m_Pics_Effect->GetVariableByName("centerY")->AsScalar();
	m_Pics_Scale = m_Pics_Effect->GetVariableByName("scale")->AsScalar();
	m_Pics_PMap  = m_Pics_Effect->GetVariableByName("gMap")->AsShaderResource();
	m_Pics_Transparency =
		m_Pics_Effect->GetVariableByName("transparency")->AsScalar();
	m_Pics_Transparency->SetFloat(1);
	D3DX11_PASS_DESC PassDesc;
	m_Pics_PTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PICVertex, 2,
									  PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_Pics_PLayout));
	HR(D3DX11CompileFromFile(_T("shader\\bigpoint.fx"), NULL, NULL, NULL,
							 "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
							 &pCode, &pError, NULL));
	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
									pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Points_Effect));
	pCode->Release();
	m_Points_PTech = m_Points_Effect->GetTechniqueByName("PointTech");
	m_Points_Width = m_Points_Effect->GetVariableByName("width")->AsScalar();
	m_Points_Height = m_Points_Effect->GetVariableByName("height")->AsScalar();
	m_Points_CenterX = m_Points_Effect->GetVariableByName("centerX")->AsScalar();
	m_Points_CenterY = m_Points_Effect->GetVariableByName("centerY")->AsScalar();
	m_Points_Scale = m_Points_Effect->GetVariableByName("scale")->AsScalar();
	m_Points_Transparency =
		m_Points_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri3;
	m_Points_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri3);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PointVertex, 3,
									  PassDescTri3.pIAInputSignature,
									  PassDescTri3.IAInputSignatureSize, &m_Points_PLayout));
	HR(D3DX11CompileFromFile(_T("shader\\CircleLine.fx"), NULL, NULL, NULL,
							 "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
							 &pCode, &pError, NULL));
	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
									pCode->GetBufferSize(), NULL, m_d3dDevice, &m_CircleLine_Effect));
	pCode->Release();
	m_CircleLine_PTech = m_CircleLine_Effect->GetTechniqueByName("PointTech");
	m_CircleLine_Width =
		m_CircleLine_Effect->GetVariableByName("width")->AsScalar();
	m_CircleLine_Height =
		m_CircleLine_Effect->GetVariableByName("height")->AsScalar();
	m_CircleLine_CenterX =
		m_CircleLine_Effect->GetVariableByName("centerX")->AsScalar();
	m_CircleLine_CenterY =
		m_CircleLine_Effect->GetVariableByName("centerY")->AsScalar();
	m_CircleLine_Scale =
		m_CircleLine_Effect->GetVariableByName("scale")->AsScalar();
	m_CircleLine_Transparency =
		m_CircleLine_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri4;
	m_CircleLine_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri4);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PointVertex, 3,
									  PassDescTri4.pIAInputSignature,
									  PassDescTri4.IAInputSignatureSize, &m_CircleLine_PLayout));
	HR(D3DX11CompileFromFile(_T("shader\\SkeletonLine.fx"), NULL, NULL, NULL,
							 "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
							 &pCode, &pError, NULL));
	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
									pCode->GetBufferSize(), NULL, m_d3dDevice, &m_SkeletonLines_Effect));
	pCode->Release();
	m_SkeletonLines_PTech = m_SkeletonLines_Effect->GetTechniqueByName("PointTech");
	m_SkeletonLines_Width =
		m_SkeletonLines_Effect->GetVariableByName("width")->AsScalar();
	m_SkeletonLines_Height =
		m_SkeletonLines_Effect->GetVariableByName("height")->AsScalar();
	m_SkeletonLines_CenterX =
		m_SkeletonLines_Effect->GetVariableByName("centerX")->AsScalar();
	m_SkeletonLines_CenterY =
		m_SkeletonLines_Effect->GetVariableByName("centerY")->AsScalar();
	m_SkeletonLines_Scale =
		m_SkeletonLines_Effect->GetVariableByName("scale")->AsScalar();
	m_SkeletonLines_Transparency =
		m_SkeletonLines_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri5;
	m_SkeletonLines_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri5);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_SkeletonLineVertex, 3,
									  PassDescTri5.pIAInputSignature,
									  PassDescTri5.IAInputSignatureSize, &m_SkeletonLines_PLayout));
}

void D3DApp_Picture::BuildPoint()
{
	ReleaseCOM(m_Pics_Buffer);
	ReleaseCOM(m_Points_Buffer);
	ReleaseCOM(m_CircleLine_Buffer);
	ReleaseCOM(m_SkeletonLines_Buffer);
	if (!m_DeviceContext)
	{
		return;
	}
	m_PicsVertices.clear();
	if (m_Pics_Texture)
	{
		PictureVertex pv;
		pv.position.x = m_PicW / 2;
		pv.position.y = m_PicH / 2;
		pv.position.z = 0;
		pv.size.x = m_PicW;
		pv.size.y = m_PicH;
		m_PicsVertices.push_back(pv);
	}
	switch (m_MouseType)
	{
	case NONE_LINE:
		break;
	case CIRCLE_LINE:
		m_CircleLineVertices.push_back(m_MousePoint);
		break;
	case VERTICAL_LINE:
		m_SkeletonLinesVertices.push_back(m_MouseVertical);
		break;
	case HORIZONTAL_LINE:
		m_SkeletonLinesVertices.push_back(m_MouseHorizontal);
		break;
	}
	if (!m_PicsVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(PictureVertex) * m_PicsVertices.size());
		m_vbd.StructureByteStride = sizeof(PictureVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_PicsVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Pics_Buffer));
	}
	if (!m_PointsVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(PointVertex) * m_PointsVertices.size());
		m_vbd.StructureByteStride = sizeof(PointVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_PointsVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Points_Buffer));
	}
	if (!m_CircleLineVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(PointVertex) * m_CircleLineVertices.size());
		m_vbd.StructureByteStride = sizeof(PointVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_CircleLineVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_CircleLine_Buffer));
	}
	if (!m_SkeletonLinesVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(SkeletonLineVertex) *
								 m_SkeletonLinesVertices.size());
		m_vbd.StructureByteStride = sizeof(SkeletonLineVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_SkeletonLinesVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_SkeletonLines_Buffer));
	}
	switch (m_MouseType)
	{
	case NONE_LINE:
		break;
	case CIRCLE_LINE:
		m_CircleLineVertices.pop_back();
		break;
	case VERTICAL_LINE:
		m_SkeletonLinesVertices.pop_back();
		break;
	case HORIZONTAL_LINE:
		m_SkeletonLinesVertices.pop_back();
		break;
	}
}

void D3DApp_Picture::LoadBlend()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// 開啟zbuffer write
	if (D3D_OK != m_d3dDevice->CreateDepthStencilState(&depth_stencil_desc,
			&m_pDepthStencil_ZWriteON))
	{
		return ;
	}
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// 關閉zbuffer write
	if (D3D_OK != m_d3dDevice->CreateDepthStencilState(&depth_stencil_desc,
			&m_pDepthStencil_ZWriteOFF))
	{
		return ;
	}
	m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteON, 0);
	CD3D11_BLEND_DESCX blend_state_desc(
		FALSE,
		FALSE,
		TRUE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL);
	// ADD混色模式
	if (D3D_OK != m_d3dDevice->CreateBlendState(&blend_state_desc,
			&m_pBlendState_ADD))
	{
		return;
	}
	blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA ;
	blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	// Alpha Blend混色模式
	if (D3D_OK != m_d3dDevice->CreateBlendState(&blend_state_desc,
			&m_pBlendState_BLEND))
	{
		return ;
	}
	float BlendFactor[4] = {0, 0, 0, 0};
	m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
}

void D3DApp_Picture::SetTexture(ID3D11ShaderResourceView* tex)
{
	if (!tex)
	{
		return;
	}
	m_Pics_Texture = tex;
	m_Pics_PMap->SetResource(tex);
	OnResize(m_ClientWidth, m_ClientHeight);
}

void D3DApp_Picture::SetPictureSize(int w, int h)
{
	if (w == h && w == 0)
	{
		m_PicW = m_ClientWidth;
		m_PicH = m_ClientHeight;
	}
	else
	{
		m_PicW = w;
		m_PicH = h;
	}
}

void D3DApp_Picture::InterSetSize(float w, float h)
{
	// if initialize ok
	if (m_Pics_Width != NULL && m_SkeletonLines_Width != NULL)
	{
		m_Pics_Width->SetFloat(w);
		m_Pics_Height->SetFloat(h);
		m_Points_Width->SetFloat(w);
		m_Points_Height->SetFloat(h);
		m_CircleLine_Width->SetFloat(w);
		m_CircleLine_Height->SetFloat(h);
		m_SkeletonLines_Width->SetFloat(w);
		m_SkeletonLines_Height->SetFloat(h);
	}
}

void D3DApp_Picture::InterSetLookCenter(float x, float y)
{
	// if initialize ok
	if (m_Pics_CenterX != NULL && m_SkeletonLines_CenterX != NULL)
	{
		m_Pics_CenterX->SetFloat(x);
		m_Pics_CenterY->SetFloat(y);
		m_Points_CenterX->SetFloat(x);
		m_Points_CenterY->SetFloat(y);
		m_CircleLine_CenterX->SetFloat(x);
		m_CircleLine_CenterY->SetFloat(y);
		m_SkeletonLines_CenterX->SetFloat(x);
		m_SkeletonLines_CenterY->SetFloat(y);
	}
}

void D3DApp_Picture::InterSetScale(float s)
{
	// if initialize ok
	if (m_Pics_Scale != NULL && m_SkeletonLines_Scale != NULL)
	{
		m_Pics_Scale->SetFloat(s);
		m_Points_Scale->SetFloat(s);
		m_CircleLine_Scale->SetFloat(s);
		m_SkeletonLines_Scale->SetFloat(s);
	}
}

void D3DApp_Picture::SetScale(float s)
{
	m_Scale = s;
}

void D3DApp_Picture::SetLookCenter(float x, float y)
{
	m_LookCenterX = x;
	m_LookCenterY = y;
	BuildPoint();
	DrawScene();
}

ID3D11Device* D3DApp_Picture::GetDevice()
{
	return m_d3dDevice;
}

ID3D11DeviceContext* D3DApp_Picture::GetDeviceContext()
{
	return m_DeviceContext;
}

void D3DApp_Picture::AddBigPoint(float x, float y, float radius,
								 D3DXVECTOR3 color)
{
	PointVertex pv;
	pv.color = color;
	pv.size.x = radius;
	pv.size.y = radius;
	pv.position.x = x;
	pv.position.y = m_PicH - y;
	m_PointsVertices.push_back(pv);
	m_CircleLineVertices.push_back(pv);
}

void D3DApp_Picture::SetMousePoint(float x, float y, float radius,
								   D3DXVECTOR3 color)
{
	m_MousePoint.color = color;
	m_MousePoint.size.x = radius;
	m_MousePoint.size.y = radius;
	m_MousePoint.position.x = x;
	m_MousePoint.position.y = m_PicH - y;
	m_MouseVertical.color = color;
	m_MouseVertical.p1.x = x;
	m_MouseVertical.p2.x = x;
	m_MouseVertical.p1.y = m_PicH - y - radius * 0.5;
	m_MouseVertical.p2.y = m_PicH - y + radius * 0.5;
	m_MouseHorizontal.color = color;
	m_MouseHorizontal.p1.x = x - radius * 0.5;
	m_MouseHorizontal.p2.x = x + radius * 0.5;
	m_MouseHorizontal.p1.y = m_PicH - y;
	m_MouseHorizontal.p2.y = m_PicH - y;
}

void D3DApp_Picture::ClearTriangles()
{
	ReleaseCOM(m_Pics_Buffer);
	m_PicsVertices.clear();
	ReleaseCOM(m_Points_Buffer);
	m_PointsVertices.clear();
	ReleaseCOM(m_CircleLine_Buffer);
	m_CircleLineVertices.clear();
// 	ReleaseCOM(m_SkeletonLines_Buffer);
// 	m_SkeletonLinesVertices.clear();
}

void D3DApp_Picture::InterDraw()
{
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView,
										   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);
	switch (m_MouseType)
	{
	case NONE_LINE:
		break;
	case CIRCLE_LINE:
		m_CircleLineVertices.push_back(m_MousePoint);
		break;
	case VERTICAL_LINE:
		m_SkeletonLinesVertices.push_back(m_MouseVertical);
		break;
	case HORIZONTAL_LINE:
		m_SkeletonLinesVertices.push_back(m_MouseHorizontal);
		break;
	}
	if (m_PicsVertices.size() > 0)
	{
		m_Pics_PMap->SetResource(m_Pics_Texture);
		UINT offset = 0;
		UINT stride2 = sizeof(PictureVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Pics_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Pics_Buffer, &stride2, &offset);
		m_Pics_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PicsVertices.size(), 0);
	}
	if (m_PointsVertices.size() > 0)
	{
		m_Points_Transparency->SetFloat(0.4);
		UINT offset = 0;
		UINT stride2 = sizeof(PointVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Points_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Points_Buffer, &stride2, &offset);
		m_Points_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PointsVertices.size(), 0);
	}
	if (m_CircleLineVertices.size() > 0)
	{
		m_CircleLine_Transparency->SetFloat(1);
		UINT offset = 0;
		UINT stride2 = sizeof(PointVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_CircleLine_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_CircleLine_Buffer, &stride2,
											&offset);
		m_CircleLine_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_CircleLineVertices.size(), 0);
	}
	if (m_SkeletonLinesVertices.size() > 0)
	{
		m_SkeletonLines_Transparency->SetFloat(1);
		UINT offset = 0;
		UINT stride2 = sizeof(SkeletonLineVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_SkeletonLines_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_SkeletonLines_Buffer, &stride2,
											&offset);
		m_SkeletonLines_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_SkeletonLinesVertices.size(), 0);
	}
	switch (m_MouseType)
	{
	case NONE_LINE:
		break;
	case CIRCLE_LINE:
		m_CircleLineVertices.pop_back();
		break;
	case VERTICAL_LINE:
		m_SkeletonLinesVertices.pop_back();
		break;
	case HORIZONTAL_LINE:
		m_SkeletonLinesVertices.pop_back();
		break;
	}
}

void D3DApp_Picture::SetLineRadius(float r)
{
	m_LineRadius = r;
}

void D3DApp_Picture::SetMouseType(Shape s)
{
	m_MouseType = s;
}

D3DApp_Picture::Shape D3DApp_Picture::GetMouseType()
{
	return m_MouseType;
}

void D3DApp_Picture::AddLine(const Line& lines, D3DXVECTOR3 color)
{
	for (int i = 0; i < lines.size(); ++i)
	{
		SkeletonLineVertex slv;
		slv.color = color;
		slv.p1.x = lines[0].x;
		slv.p1.y = m_PicH - lines[0].y;
		slv.p2.x = lines[1].x;
		slv.p2.y = m_PicH - lines[1].y;
		m_SkeletonLinesVertices.push_back(slv);
	}
}
