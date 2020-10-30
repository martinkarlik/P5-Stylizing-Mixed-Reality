// Copyright 2019-2020 Varjo Technologies Oy. All rights reserved.

#pragma once

#include "Globals.hpp"
#include "LayerView.hpp"
#include "D3D11Renderer.hpp"

namespace VarjoExamples
{
//! Layer view implementation for D3D11 renderer
class D3D11LayerView final : public LayerView
{
public:
    //! Constructor
    D3D11LayerView(varjo_Session* session, D3D11Renderer& renderer);

    //! Static function for getting DXGI adapter used by Varjo compositor
    static ComPtr<IDXGIAdapter> getAdapter(varjo_Session* session);
};

}  // namespace VarjoExamples