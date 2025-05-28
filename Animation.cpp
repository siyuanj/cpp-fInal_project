#include "Animation.h"
#include <windows.h>
#include <stdexcept> // 为了使用 std::runtime_error

void putimage_alpha(int x, int y, IMAGE* img) {
    if (!img) {
        // 处理img为空的情况，例如打印错误信息或直接返回
        fprintf(stderr, "Error: putimage_alpha called with null IMAGE pointer.\n");
        return;
    }
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
        GetImageHDC(img), 0, 0, w, h,
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

Atlas::Atlas(LPCTSTR path, int num) {
    TCHAR path_file[256];
    for (size_t i = 0; i < num; i++) {
        int result = _stprintf_s(path_file, sizeof(path_file) / sizeof(TCHAR), path, i);
        if (result < 0) {
            // _stprintf_s 失败，处理错误
            fprintf(stderr, "Error: _stprintf_s failed to format path.\n");
            // 可以选择抛出异常或以其他方式处理错误
            throw std::runtime_error("Failed to format image path.");
        }

        IMAGE* picture = new IMAGE;
        if (picture == nullptr) {
            // new IMAGE 失败，内存分配失败
            fprintf(stderr, "Error: Failed to allocate memory for IMAGE object for path: %s\n", path_file);
            // 抛出异常或采取其他错误处理措施
            throw std::bad_alloc();
        }

        // 假设 loadimage 返回一个状态或错误码，或者在失败时设置全局错误状态
        // EasyX 的 loadimage 通常在失败时会使 IMAGE 对象处于无效状态，
        // 并且可能通过 GetLastError() 或特定的 EasyX 错误函数提供更多信息。
        // 这里我们直接调用，如果它导致崩溃，说明问题可能在 loadimage 内部或其参数。
        loadimage(picture, path_file);

        // 检查 IMAGE 对象是否有效（如果 IMAGE 类提供了这样的方法）
        // 例如，可以检查宽度和高度是否大于0
        if (picture->getwidth() == 0 || picture->getheight() == 0) {
            fprintf(stderr, "Error: loadimage failed for path: %s. Image might be missing or corrupted.\n", path_file);
            // 清理已分配的 picture 对象
            delete picture;
            // 可以选择抛出异常或记录错误并跳过此帧
            // throw std::runtime_error("Failed to load image.");
            continue; // 跳过这个损坏的或缺失的图像
        }

        frame_list.push_back(picture);
    }
}

Atlas::~Atlas() {
    for (size_t i = 0; i < frame_list.size(); i++) {
        delete frame_list[i];
    }
}

Animation::Animation(Atlas* atlas, int jiange) 
    : anim_atlas(atlas), internal(jiange), idx_frame(0), timer(0) {
    if (anim_atlas == nullptr) {
        fprintf(stderr, "Error: Animation created with null Atlas pointer.\n");
        throw std::invalid_argument("Atlas pointer cannot be null for Animation.");
    }
    if (anim_atlas->frame_list.empty()) {
        fprintf(stderr, "Warning: Animation created with an Atlas that has no frames.\n");
        // 这种情况下，showimage 中的取模操作可能会有问题，如果 frame_list.size() 为0
        // idx_frame 将始终为0，但访问 frame_list[0] 会越界。
        // 最好在 showimage 中也处理这种情况。
    }
}

Animation::~Animation() {}

void Animation::showimage(int x, int y, int delta) {
    if (anim_atlas == nullptr || anim_atlas->frame_list.empty()) {
        // 如果图集为空或没有帧，则不执行任何操作或打印错误
        // fprintf(stderr, "Error: Cannot show image, Atlas is null or empty.\n");
        return;
    }
    timer += delta;
    if (timer >= internal) {
        //idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size(); 有溢出风险
        idx_frame = (static_cast<size_t>(idx_frame) + 1) % anim_atlas->frame_list.size();
        // 使用 static_cast<size_t> 将 idx_frame 转换为与 anim_atlas->frame_list.size() 相同的类型（size_t）
        // 以确保计算在更宽的类型范围内进行，避免溢出。
        timer = 0;
    }
    
    // 在访问frame_list之前，再次确认idx_frame是有效的
    if (idx_frame < anim_atlas->frame_list.size()) {
        putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
    } else {
        // 这种情况理论上不应该发生，如果取模运算正确的话
        fprintf(stderr, "Error: idx_frame out of bounds in showimage.\n");
    }
}