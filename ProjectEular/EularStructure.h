#pragma once
#ifndef __HALF_EDGES__
#define __HALF_EDGES__
#include<iostream>
#include<math.h>
#include<stdio.h>
#include<vector>
using namespace std;
struct Solid;
struct Face;
struct Loop;
struct Edge;
struct HalfEdges;
struct Vertex;
struct Point;
//������ṹ
struct Solid {
	int S_id;
	Face* face;
	Edge* edge;
	Solid* next;
	Solid* pre;
	Vertex* ver;
	Solid(){
		next = pre = NULL;
		face = NULL;
		edge = NULL;
		ver = NULL;
		static int id = 0;
		S_id = id++;
	}
};
//������ṹ
struct Face {
	int F_id;
	Solid* solid;
	Loop* loop;
	Face* pre;
	Face* next;
	double pos[3];
	Face() {
		next = pre = NULL;
		solid = NULL;
		loop = NULL;
		pos[0] = pos[1] = pos[2] = 0.0;
		static int id = 0;
		F_id = id++;
	}
};
//���廷�ṹ
struct Loop {
	int L_id;
	HalfEdges* halfedge;
	Face* face;
	Loop* next;
	Loop* pre;
	Loop(){
		static int id = 0;
		L_id = id++;
		next = pre = NULL;
		face = NULL;
		halfedge = NULL;
	}
};
//����߽ṹ
struct Edge {
	HalfEdges* left;
	HalfEdges* right;
	Edge* next;
	Edge* pre;
    int E_id;
	Edge() {
		left = right = NULL;
		pre = next = NULL;
		static int id = 0;
		E_id = id++;
	}
};
//�����߽ṹ
struct HalfEdges {
	int HF_id;
	Edge* edge;
	Loop* lp;
	HalfEdges* pre;
	HalfEdges* next;
	HalfEdges* brother;
	Vertex* ver;
	HalfEdges(){
		static int id = 0;
		HF_id = id++;
		edge = NULL;
		lp = NULL;
		ver = NULL;
		next = pre = brother = NULL;
	}
};
//�����ṹ
struct Vertex {
	int V_id;
	double coordinate[3];
	Vertex* next;
	Vertex* pre;
	Vertex(){
		next = pre = NULL;
		static int id = 0;
		V_id = id++;
		coordinate[0] = 0;
		coordinate[1] = 0;
		coordinate[2] = 0;
	}
};
#endif