#pragma once
#include <graphics.h>      // EasyX图形库
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // 标准模板库vector容器
#include <cmath>          // 数学函数库
#include <conio.h>         // 控制台输入输出
#include <random>
#include <algorithm>

#include "Animation.h"        // 动画系统


class tombstone {

private:
	int hp = 500;
	POINT position;
	Atlas* tomb_atlas = new Atlas(_T("img/tombstone.png"));
	// 考虑静态？
	Animation* anim;


public:
	tombstone(POINT pos) : position(pos) {
		anim = new Animation(tomb_atlas, 50);

	}
	void draw() {
		putimage_alpha(position.x, position.y, tomb_atlas->frame_list[0]);
	}

	POINT getPosition() {
		return position;
	}




};
