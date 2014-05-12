/*
 * rasterizer.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: jwpilly
 */

#include "rasterizer.h"

rasterizer::rasterizer() {

}

void rasterizer::draw_span(screen* s, span* a, int y) {
	int x_diff = (int)a->get_x_2() - (int)a->get_x_1();
	if (x_diff == 0) {
		return;
	}
	color* c_diff = a->get_b_color()->clone();
	c_diff->subtract(a->get_a_color());
	float factor = (float)0;
	float factor_step = (float)1 / (float)x_diff;
	for (int x = (int)a->get_x_1(); x < (int)a->get_x_2(); x++) {
		vector2* pixel = new vector2((float)x, (float)y);
		color* c = a->get_a_color()->clone();
		color* c_1 = c_diff->clone();
		c_1->multiply(factor);
		c->add(c_1);
		this->set_pixel(s, pixel, c);
		factor += factor_step;
		delete pixel;
		delete c;
		delete c_1;
	}
	delete c_diff;
}

void rasterizer::draw_edge_span(screen* s, edge* a, edge* b) {
	float y_diff_1 = (float)(a->get_b()->get_y() - a->get_a()->get_y());
	if (y_diff_1 == (float)0) {
		return;
	}
	float y_diff_2 = (float)(b->get_b()->get_y() - b->get_a()->get_y());
	if (y_diff_2 == (float)0) {
		return;
	}
	float x_diff_1 = (float)(a->get_b()->get_x() - a->get_a()->get_x());
	float x_diff_2 = (float)(b->get_b()->get_x() - b->get_a()->get_x());
	color* e_1 = a->get_b_color()->clone();
	e_1->subtract(a->get_a_color());
	color* e_2 = b->get_b_color()->clone();
	e_2->subtract(b->get_a_color());
	float factor_1 = (float)(b->get_a()->get_y() - a->get_a()->get_y()) / y_diff_1;
	float factor_step_1 = (float)1 / y_diff_1;
	float factor_2 = (float)0;
	float factor_step_2 = (float)1/y_diff_2;
	for (int y = (int)b->get_a()->get_y(); y < (int)b->get_b()->get_y(); y++) {
		color* c = a->get_a_color()->clone();
		color* c_1 = e_1->clone();
		c_1->multiply(factor_1);
		c->add(c_1);
		int c_x_1 = a->get_a()->get_x() + (int)(x_diff_1 * factor_1);
		color* d = b->get_a_color()->clone();
		color* d_1 = e_2->clone();
		d_1->multiply(factor_2);
		d->add(d_1);
		int c_x_2 = b->get_a()->get_x() + (int)(x_diff_2 * factor_2);
		span* sp = new span(*c, c_x_1, *d, c_x_2);
		draw_span(s, sp, y);
		delete c;
		delete c_1;
		delete d;
		delete d_1;
		delete sp;
		factor_1 += factor_step_1;
		factor_2 += factor_step_2;
	}
	delete e_1;
	delete e_2;
}

void rasterizer::draw_triangle_fill(screen* s, vector2* a, color* a_color, vector2* b, color* b_color, vector2* c, color* c_color) {
	edge* edges[3];
	edges[0] = new edge(*a, *a_color, *b, *b_color);
	edges[1] = new edge(*b, *b_color, *c, *c_color);
	edges[2] = new edge(*c, *c_color, *a, *a_color);
	float max_length = (float)0;
	int long_edge = 0;
	for (int i = 0; i < 3; i++) {
		float length = edges[i]->get_b()->get_y() - edges[i]->get_a()->get_y();
		if (length > max_length) {
			max_length = length;
			long_edge = i;
		}
	}
	int short_1 = (long_edge + 1) % 3;
	int short_2 = (long_edge + 2) % 3;
	printf("long %d short %d %d\n", long_edge, short_1, short_2);
	draw_edge_span(s, edges[long_edge], edges[short_1]);
	draw_edge_span(s, edges[long_edge], edges[short_2]);
}

void rasterizer::draw_triangle_wire(screen* s, vector2* a, color* a_color, vector2* b, color* b_color, vector2* c, color* c_color) {
	draw_line(s, a, a_color, b, b_color);
	draw_line(s, b, b_color, c, c_color);
	draw_line(s, c, c_color, a, a_color);
}

void rasterizer::draw_line(screen* s, vector2* a, vector2* b, color* c) {
	int w = (int) (b->get_x() - a->get_x());
	int h = (int) (b->get_y() - a->get_y());
	float m = h / (float) w;
	float j = a->get_y();
	for (int i = a->get_x(); i < b->get_x(); i++) {
		s->set_pixel(i, j, c);
		j += m;
	}
}

void rasterizer::draw_line(screen* s, vector2* a, color* a_color, vector2* b, color* b_color) {
	float x_diff = b->get_x() - a->get_x();
	float y_diff = b->get_y() - a->get_y();
	if (x_diff == 0 && y_diff == 0) {
		s->set_pixel(a->get_x(), a->get_y(), a_color);
		return;
	}
	if (fabs(x_diff) > fabs(y_diff)) {
		float x_min, x_max;
		if (a->get_x() < b->get_x()) {
			x_min = a->get_x();
			x_max = b->get_x();
		} else {
			x_min = b->get_x();
			x_max = a->get_x();
		}
		float slope = y_diff / x_diff;
		for (float x = x_min; x <= x_max; x += (float) 1) {
			float y = a->get_y() + ((x - a->get_x()) * slope);
			color* c = a_color->clone();
			float c_s = (x - a->get_x()) / x_diff;
			color* d = b_color->clone();
			d->subtract(a_color);
			d->multiply(c_s);
			c->add(d);
			s->set_pixel((int) x, (int) y, c);
			delete c;
			delete d;
		}
	} else {
		float y_min, y_max;
		if (a->get_y() < b->get_y()) {
			y_min = a->get_y();
			y_max = b->get_y();
		} else {
			y_min = b->get_y();
			y_max = a->get_y();
		}
		float slope = x_diff / y_diff;
		for (float y = y_min; y <= y_max; y += (float) 1) {
			float x = a->get_x() + ((y - a->get_y()) * slope);
			color* c = a_color->clone();
			float c_s = ((y - a->get_y()) / y_diff);
			color* d = b_color->clone();
			d->subtract(a_color);
			d->multiply(c_s);
			c->add(d);
			s->set_pixel((int) x, (int) y, c);
			delete c;
			delete d;
		}
	}
}

void rasterizer::set_pixel(screen* s, vector2* p, color* c) {
	s->set_pixel((int) p->get_x(), (int) p->get_y(), c);
}

rasterizer::~rasterizer() {
}

