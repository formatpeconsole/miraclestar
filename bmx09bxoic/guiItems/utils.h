#pragma once
#include <string>
#include "../gui/item.h"

namespace gui::items
{
template<typename T>
inline std::string getPreviewItemName(const BindValues<T>& value)
{
    if (value.bindKey > 0 && value.bindMode != -1)
    {
        return binds::ImGui_ImplWin32_VKeyToString(value.bindKey)
            + " - "
            + binds::getBindMode(value.bindMode + 1);
    }

    return "New Bind ##" + value.name;
}

template<typename T>
inline std::string getBindsComboLabel(const Item<T>& item, const BindPreview<T>& preview)
{
    if (item.binds.empty()
        || !preview.selectedBind.has_value() 
        || preview.selectedBind.value() == item.binds.end())
        return "No binds.";

    return preview.selectedBind.value()->previewName;
}
}