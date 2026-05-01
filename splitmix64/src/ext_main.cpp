#include "ext.hpp"
#include <dmsdk/extension/extension.hpp>

static dmExtension::Result InitializeExtension(dmExtension::Params *params)
{
    Splitmix64Ext::LuaOpen(params->m_L);
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(splitmix64, "splitmix64", 0, 0, InitializeExtension, 0, 0, 0)
