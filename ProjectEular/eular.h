#pragma once
#include <cstdio>
#include <vector>
#include<iostream>
#include<math.h>
#include"EularStructure.h"
//add系列的函数基本原理都是迭代指针直到链表尾部，然后把对应结构链接上去

//把一个loop添加到face上
void addloop(Face* face, Loop* newloop) {
	Loop* temploop = face->loop;
	if (!temploop) {
		face->loop = newloop;
	}
	else {
		while (temploop->next) {
			temploop = temploop->next;
		}
		temploop->next = newloop;
		newloop->pre = temploop;
	}
	newloop->face = face;
}
//把半边添加到loop上
void addhalfedge(Loop* loop, HalfEdges* hf) {
	HalfEdges* temphf = loop->halfedge;
	if (!temphf) {
		loop->halfedge = hf;
	}
	else {
		while (temphf->next) {
			temphf = temphf->next;
		}
		temphf->next = hf;
		hf->pre = temphf;
	}
	hf->lp = loop;
}
//把一个面加到体上
void addface(Face* face, Solid* solid) {
	Face* f = solid->face;
	if (!f)solid->face = face;
	else {
		while (f->next) {
			f = f->next;
		}
		f->next = face;
		face->pre = f;
	}
	face->solid = solid;
}
//把edge添加到solid上
void addedge(Solid* solid, Edge* edge) {
	Edge* tempedge = solid->edge;
	if (!tempedge) {
		solid->edge = edge;
	}
	else {
		while (tempedge->next) {
			tempedge = edge;
		}
		tempedge->next = edge;
		edge->pre = tempedge;
	}
}
//把某点添加到solid上
void addvertex(Solid* solid, Vertex* ver) {
	Vertex* tempver = solid->ver;
	if (!tempver) {
		solid->ver = ver;
	}
	else {
		while (tempver->next) {
			tempver = tempver->next;
		}
		tempver->next = ver;
		ver->pre = tempver;
	}
}
//判断某一点是否在loop内
bool isInloop(Loop* loop, Vertex* v) {
	if (!v) {
		return false;
	}
	HalfEdges* temphf = loop->halfedge;
	while (temphf) {
		if (temphf->ver == v) {
			return true;
		}
		temphf = temphf->next;
		if (temphf == loop->halfedge) {
			return false;
		}
	}
	return false;
}
//在环中寻找某条半边
HalfEdges* findhalfedge(Loop* loop, Vertex* v) {
	if (!v)return NULL;
	HalfEdges* temphf = loop->halfedge;
	while (temphf) {
		if (temphf->ver == v)return temphf;
		temphf = temphf->next;
		if (temphf == loop->halfedge)return NULL;
	}
	return NULL;
}
//在体中寻找某点
Vertex* findvertex(Solid* solid, double* point) {
	Vertex* tempver = solid->ver;
	while (tempver) {
		if (tempver->coordinate[0] == point[0] && tempver->coordinate[1] == point[1] && tempver->coordinate[2] == point[2]) {
			return tempver;
		}
		tempver = tempver->next;
	}
	return NULL;
}
//在体中寻找某面
Face* findfaceend(Solid* solid) {
	Face* tempface = solid->face;
	while (tempface->next) {
		tempface = tempface->next;
	}
	return tempface;
}
//在体中找寻某边
Edge* findedge(Solid* solid, Vertex* v1, Vertex* v2) {
	Edge* tempedge = solid->edge;
	while (tempedge) {
		if (tempedge->left->ver == v1 && tempedge->right->ver == v2 || tempedge->left->ver == v2 && tempedge->right->ver == v1) {
			return tempedge;
		}
		tempedge = tempedge->next;
	}
	return tempedge;
}
//在渲染空间设置某点的坐标
void setPos(Vertex* vt, double* point) {
	vt->coordinate[0] = point[0];
	vt->coordinate[1] = point[1];
	vt->coordinate[2] = point[2];
}
//以下是欧拉操作的算子

Solid* mvfs(double* point) {
	Solid* solid = new Solid();
	Loop* loop = new Loop();
	Face* face = new Face();
	Vertex* vertex = new Vertex();
	setPos(vertex, point);
	addface(face, solid);
	addloop(face, loop);
	addvertex(solid, vertex);
	return solid;
}
//在两个点间连一条线
HalfEdges* mev(double* p1, double* p2, Loop* loop) {
	Solid* solid = loop->face->solid;
	Edge* edge = new Edge();
	Vertex* vertex = new Vertex();
	HalfEdges* left = new HalfEdges();
	HalfEdges* right = new HalfEdges();
	Vertex* vtmp = findvertex(solid, p1);
	if (!vtmp) {
		return NULL;
	}
	left->ver = vtmp;
	setPos(vertex, p2);
	right->ver = vertex;
	left->next = left->brother = right;
	right->pre = right->brother = left;

	HalfEdges* temphf = loop->halfedge;
	//如果没有在环中定义半边，就在此新定义一个半边
	if (!temphf) {
		loop->halfedge = left;
		left->pre = right;
		right->next = left;
	}
	else {
		//找到和需要连接点关联的半边
		while (temphf->next->ver != vtmp) {
			temphf = temphf->next;
		}
		right->next = temphf->next;
		temphf->next->pre = right;
		temphf->next = left;
		left->pre = temphf;
	}
	left->lp = right->lp = loop;
	left->edge = right->edge = edge;
	edge->left = left;
	edge->right = right;
	addedge(solid, edge);
	addvertex(solid, vertex);

	return left;
}
//在头尾两点连线，生成一个环，没有新增的点
Loop* mef(double* p1, double* p2, Loop* loop) {
	Solid* solid = loop->face->solid;
	Vertex* v1 = findvertex(solid, p1);
	if (!v1)return NULL;
	Vertex* v2 = findvertex(solid, p2);
	if (!v2)return NULL;
	if (!(isInloop(loop, v1) && isInloop(loop, v2))) {
		return NULL;
	}
	Face* face = new Face();
	Loop* loops = new Loop();
	Edge* edge = new Edge();
	HalfEdges* left = new HalfEdges();
	HalfEdges* right = new HalfEdges();
	HalfEdges* templeft = findhalfedge(loop, v1);
	HalfEdges* tempright = findhalfedge(loop, v2);
	addhalfedge(loops, right);
	HalfEdges* temphf = templeft;
	while (temphf) {
		if (temphf == tempright)break;
		temphf->lp = loops;
		temphf = temphf->next;
	}
	//使用3个temp将两点之间的两个halfedge链接起来
	right->next = templeft;
	if (!temphf)return loops;
	temphf = templeft->pre;
	templeft->pre = right;
	tempright->pre->next = right;
	right->pre = tempright->pre;

	temphf->next = left;
	left->pre = temphf;
	left->next = tempright;
	tempright->pre = left;
	left->lp = loop;
	loop->halfedge = left;

	left->edge = right->edge = edge;
	edge->left = left;
	edge->right = right;
	left->brother = right;
	right->brother = left;
	left->ver = v1;
	right->ver = v2;
	addedge(solid, edge);
	addface(face, solid);
	addloop(face, loops);
	return loops;
}
void kvfs(double *p,Solid *solid) {
	Vertex* vertex = findvertex(solid,p);
	Face* face = solid->face;
	Loop* loop = face->loop;
	delete vertex;
	delete loop;
	delete face;
	delete solid;
}
void kev(double *p1,double *p2,Solid *solid) {
	Vertex* v1 = findvertex(solid, p1);
	Vertex* v2 = findvertex(solid, p2);
	Edge* dedge = findedge(solid,v1,v2);
	HalfEdges* dleft = dedge->left;
	HalfEdges* dright = dedge->right;
	Vertex* delv = v2;
	Face* face = solid->face;
	while (face) {
		Loop* loop = face->loop;
		while (loop) {
			if (findhalfedge(loop, delv)) {
				findhalfedge(loop, delv)->ver = v1;
			}
			loop = loop->next;
		}
		face = face->next;
	}
	dleft->lp->halfedge = dleft->next;
	dright->lp->halfedge = dright->next;
	dleft->next->pre = dleft->pre;
	dleft->next->pre = dleft->next;
	dright->next->pre = dright->pre;
	dright->next->pre = dright->next;
	delete dleft;
	delete dright;
	if (dedge->next && dedge->pre) {
		dedge->next->pre = dedge->pre;
		dedge->pre->next = dedge->next;
	}
	else if (dedge->next == NULL&&dedge->pre != NULL) {
		dedge->pre->next = NULL;
	}
	else if (dedge->pre == NULL&&dedge->next != NULL) {
		dedge->next->pre = NULL;
		solid->edge = dedge->next;
	}
	else {
		solid->edge = NULL;
	}
	delete dedge;
	if (delv->next && delv->pre) {
		delv->next->pre = delv->pre;
		delv->pre->next = delv->next;
	}
	else if (delv->next==NULL&&delv->pre!=NULL) {
		delv->pre->next = NULL;
	}
	else if (delv->pre==NULL&&delv->next!=NULL) {
		delv->next->pre = NULL;
		solid->ver = delv->next;
	}
	else {
		solid->ver = NULL;
	}
	delete delv;
}
void kef(double *p1,double *p2,Loop *loop) {
	Solid* solid = loop->face->solid;
	Vertex* v1 = findvertex(solid, p1);
	Vertex* v2 = findvertex(solid, p2);
	HalfEdges* dleft = findhalfedge(loop, v1);
	HalfEdges* dright = dleft->brother;
	Edge* dedge = dleft->edge;
	Loop* dloop = dright->lp;
	Face* dface = dloop->face;
	HalfEdges* temphf = dright->next;
	while (temphf) {
		if (temphf == dright)break;
		temphf->lp = loop;
		temphf = temphf->next;
	}
	dleft->next->pre = dright->pre;
	dright->pre->next = dleft->next;
	dleft->pre->next = dright->next;
	if (!dright->next)return;
	dright->next->pre = dleft->pre;
	loop->halfedge = dleft->next;
	delete dleft;
	delete dright;
	if (dedge->next&&dedge->pre) {
		dedge->next->pre = dedge->pre;
		dedge->pre->next = dedge->next;
	}
	else if(dedge->next==NULL&&dedge->pre!=NULL){
		dedge->pre->next = NULL;
	}
	else if (dedge->pre==NULL&&dedge->next!=NULL) {
		dedge->next->pre = NULL;
		solid->edge = dedge->next;
	}
	else {
		solid->edge = NULL;
	}
	delete dedge;
	if (dloop->next && dloop->pre) {
		dloop->next->pre = dloop->pre;
		dloop->pre->next = dloop->next;
	}
	else if (dloop->next == NULL && dloop->pre != NULL)
	{
		dloop->pre->next = NULL;
	}
	else if (dloop->pre == NULL && dloop->next != NULL)
	{
		dloop->next->pre = NULL;
		dloop->face->loop = dloop->next;
	}
	else
	{
		dloop->face->loop = NULL;
	}
	delete dloop;
	if (dface->next && dface->pre)
	{
		dface->next->pre = dface->pre;
		dface->pre->next = dface->next;
	}
	else if (dface->next == NULL && dface->pre != NULL)
	{
		dface->pre->next = NULL;
	}
	else if (dface->pre == NULL && dface->next != NULL)
	{
		dface->next->pre = NULL;
		solid->face = dface->next;
	}
	else
	{
		solid->face = NULL;
	}
	delete dface;
}
Loop* kemr(double *p1,double*p2,Loop *loop) {
	Solid* solid = loop->face->solid;
	Vertex* v1 = findvertex(solid, p1);
	Vertex* v2 = findvertex(solid, p2);
	if (!v1 || !v2)return NULL;
	if (!(isInloop(loop, v1) && isInloop(loop, v2)))return NULL;
	HalfEdges* left = loop->halfedge;
	while (left) {
		if (left->ver == v1 && left->next->ver == v2) {
			break;
		}
		left = left->next;
	}
	Loop* newloop = new Loop();
	if (!left)return newloop;
	HalfEdges* right = left->brother;
	if (left->next != right) {
		addhalfedge(newloop,left->next);
		if (!left->next)return newloop;
		HalfEdges* temphf = left->next->next;
		while (temphf) {
			if (temphf == right)break;
			temphf->lp = newloop;
			temphf = temphf->next;
		}
		left->next->pre = right->pre;
		right->pre->next = left->next;
	}
	addloop(loop->face,newloop);
	loop->halfedge = left->pre;
	if (!right)return newloop;
	left->pre->next = right->next;
	right->next->pre = left->pre;
	Edge* tedge = solid->edge;
	while (tedge) {
		if (tedge->left == left || tedge->right == left)break;
		tedge = tedge->next;
	}
	if (!tedge) {
		solid->edge = NULL;
	}
	else if (tedge->next && tedge->pre) {
		tedge->next->pre = tedge->pre;
		tedge->pre->next = tedge->next;
	}
	else if (tedge->next == NULL && tedge->pre != NULL)
	{
		tedge->pre->next = NULL;
	}
	else if (tedge->pre == NULL && tedge->next != NULL)
	{
		tedge->next->pre = NULL;
		solid->edge = tedge->next;
	}
	else
	{
		solid->edge = NULL;
	}
	delete tedge;
	delete left;
	delete right;

	return newloop;
}
HalfEdges* mekr(double *p1,double *p2,Loop *loop,Loop *dloop) {
	Solid* solid = loop->face->solid;
	Vertex* v1 = findvertex(solid,p1);
	Vertex* v2 = findvertex(solid, p2);
	HalfEdges* left = new HalfEdges();
	HalfEdges* right = new HalfEdges();
	Edge* edge = new Edge();
	left->ver = v1;
	right->ver = v2;
	left->brother = right;
	right->brother = left;
	left->edge = right->edge = edge;
	edge->left = left;
	edge->right = right;
	left->lp = right->lp = loop;
	HalfEdges* tempright = findhalfedge(dloop,v2);
	HalfEdges* temphf = tempright;
	while (temphf) {
		temphf->lp = loop;
		temphf = temphf->next;
		if (temphf == tempright)break;
	}
	HalfEdges* templeft = findhalfedge(loop,v1);
	templeft->pre->next = left;
	left->pre = templeft->pre;
	left->next = tempright;
	if(!temphf)return left;
	temphf = tempright->pre;
	tempright->pre = left;
	temphf->next = right;
	right->pre = temphf;
	right->next = templeft;
	templeft->pre = right;
	addedge(solid,edge);
	if (!dloop) {
	}
	else if (dloop->next && dloop->pre)
	{
		dloop->next->pre = dloop->pre;
		dloop->pre->next = dloop->next;
	}
	else if (dloop->next == NULL && dloop->pre != NULL)
	{
		dloop->pre->next = NULL;
	}
	else if (dloop->pre == NULL && dloop->next != NULL)
	{
		dloop->next->pre = NULL;
		dloop->face->loop = dloop->next;
	}
	else
	{
		dloop->face->loop = NULL;
	}
	delete dloop;

	return left;
}
void kfmrh(Loop *outloop,Loop *inloop) {
	Face* tempface = inloop->face;
	addloop(outloop->face,inloop);
	if (tempface->next && tempface->pre)
	{
		tempface->next->pre = tempface->pre;
		tempface->pre->next = tempface->next;
	}
	else if (tempface->next == NULL && tempface->pre != NULL)
	{
		tempface->pre->next = NULL;
	}
	else if (tempface->pre == NULL && tempface->next != NULL)
	{
		tempface->next->pre = NULL;
		outloop->face->solid->face = tempface->next;
	}
	else
	{
		outloop->face->solid->face = NULL;
	}
	delete tempface;
}
Face* mfkrh(Loop* outloop, Loop* inloop) {
	if (inloop->next && inloop->pre)
	{
		inloop->next->pre = inloop->pre;
		inloop->pre->next = inloop->next;
	}
	else if (inloop->next == NULL && inloop->pre != NULL)
	{
		inloop->pre->next = NULL;
	}
	else if (inloop->pre == NULL && inloop->next != NULL)
	{
		inloop->next->pre = NULL;
		inloop->face->solid->face->loop = inloop->next;
	}
	else
	{
		inloop->face->solid->face->loop = NULL;
	}
	Face* face = new Face();
	addface(face, outloop->face->solid);
	addloop(face,inloop);
	return face;
}
//扫成操作
void sweep(Face *face,double *pos) {
	double p1[3], p2[3], start[3], ed[3];
	Loop* temploop = face->loop;
	while (temploop!=NULL) {
		HalfEdges* temphf = temploop->halfedge;
		HalfEdges* next = temphf->next;
		HalfEdges* pre = temphf->pre;
		Vertex* vertex = temphf->ver;
		p1[0] = vertex->coordinate[0];
		p1[1] = vertex->coordinate[1];
		p1[2] = vertex->coordinate[2];
		start[0] = ed[0] = p2[0] = p1[0] + pos[0];
		start[1] = ed[1] = p2[1] = p1[1] + pos[1];
		start[2] = ed[2] = p2[2] = p1[2] + pos[2];
		mev(p1,p2,temploop);
		if (temphf == pre)temphf = NULL;
		else {
			temphf = next;
			if (!next)break;
			next = temphf->next;
		}
		while (temphf) {
			Vertex* vs = temphf->ver;
			p1[0] = vs->coordinate[0];
			p1[1] = vs->coordinate[1];
			p1[2] = vs->coordinate[2];
			p2[0] = p1[0] + pos[0];
			p2[1] = p1[1] + pos[1];
			p2[2] = p1[2] + pos[2];
			mev(p1,p2,temploop);
			mef(ed,p2,temploop);
			ed[0] = p2[0];
			ed[1] = p2[1];
			ed[2] = p2[2];
			if (temphf==pre) {
				mef(ed,start,temploop);
				temphf = NULL;
			}
			else {
				temphf = next;
				next = temphf->next;
			}
		}
		temploop = temploop->next;
	}
}