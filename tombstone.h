#pragma once
#include <graphics.h>      // EasyXͼ�ο�
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // ��׼ģ���vector����
#include <cmath>          // ��ѧ������
#include <conio.h>         // ����̨�������
#include <random>
#include <algorithm>

#include "Animation.h"        // ����ϵͳ


class tombstone {

private:
	int hp = 500;
	POINT position;
	Atlas* tomb_atlas = new Atlas(_T("img/tombstone.png"));
	// ���Ǿ�̬��
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
