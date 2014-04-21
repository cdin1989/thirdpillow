/*
 * camera.h
 *
 *  Created on: Apr 10, 2014
 *      Author: jwpilly
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <stdlib.h>
#include <stdio.h>

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix4.h"

class camera {
public:
	vector3 y_axis;
	void set_position(vector3* p);
	vector3* get_position();
	vector3* get_left();
	vector3* get_right();
	vector3* get_forward();
	vector3* get_up();
	void rotate_x(float degree);
	void rotate_y(float degree);
	void move(vector3* direction, float amount);
	static int get_index_3d(int x, int y, int z, int wide, int thick);
	camera(int render_width, int render_height);
	camera(int render_width, int render_height, vector3 pos, vector3 forward, vector3 up);

	virtual ~camera();
private:
	int render_width;
	int render_height;
	vector3 position;
	vector3 forward;
	vector3 up;
	bool active;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int min_z;
	int max_z;
	float z_near;
	float z_far;
	float fov;

	void initialize();
	void cleanup();
};

#endif /* CAMERA_H_ */
