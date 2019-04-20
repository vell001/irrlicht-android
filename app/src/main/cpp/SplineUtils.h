//
// Created by Administrator on 2019/4/20 0020.
//
#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;

static void genVertices(array<S3DVertex> &vertices, array<u16> &indices,
                        const array<vector3df> &keyPoints, f32 thickness = 1.0f) {
    if (keyPoints.size() < 2) {
        return;
    }

    vector3df vertexNormal = vector3df(0, 0, -1);
    SColor vertexColor = SColor(100, 255, 255, 255);

    for (int i = 0; i < keyPoints.size() - 1; i++) {
        const vector3df &start = keyPoints[i];
        const vector3df &end = keyPoints[i + 1];

        vector3df forward = (end - start).normalize() * thickness;
        vector3df normal = forward; // 垂直向量
        normal.rotateXYBy(-90);

        // 计算start半圆
        vector3df a = start - normal;
        vector3df b = start + normal;
        vector3df c = b - forward;
        vector3df d = a - forward;

        u16 aIndex = (u16)vertices.size();
        vertices.push_back(S3DVertex(a, vertexNormal, vertexColor, vector2df(0.25, 1)));
        vertices.push_back(S3DVertex(b, vertexNormal, vertexColor, vector2df(0.25, 0)));
        vertices.push_back(S3DVertex(c, vertexNormal, vertexColor, vector2df(0, 0)));
        vertices.push_back(S3DVertex(d, vertexNormal, vertexColor, vector2df(0, 1)));

        indices.push_back(aIndex);
        indices.push_back(aIndex+(u16)1);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)3);
        indices.push_back(aIndex);

        // 计算主体矩形
        d = a;
        c = b;
        a = end - normal;
        b = end + normal;

        aIndex = (u16)vertices.size();
        vertices.push_back(S3DVertex(a, vertexNormal, vertexColor, vector2df(1, 1)));
        vertices.push_back(S3DVertex(b, vertexNormal, vertexColor, vector2df(1, 0)));
        vertices.push_back(S3DVertex(c, vertexNormal, vertexColor, vector2df(0.5, 0)));
        vertices.push_back(S3DVertex(d, vertexNormal, vertexColor, vector2df(0.5, 1)));

        indices.push_back(aIndex);
        indices.push_back(aIndex+(u16)1);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)3);
        indices.push_back(aIndex);

        // 计算end半圆
        d = a;
        c = b;
        a = d + forward;
        b = c + forward;

        aIndex = (u16)vertices.size();
        vertices.push_back(S3DVertex(a, vertexNormal, vertexColor, vector2df(0.5, 1)));
        vertices.push_back(S3DVertex(b, vertexNormal, vertexColor, vector2df(0.5, 0)));
        vertices.push_back(S3DVertex(c, vertexNormal, vertexColor, vector2df(0.25, 0)));
        vertices.push_back(S3DVertex(d, vertexNormal, vertexColor, vector2df(0.25, 1)));

        indices.push_back(aIndex);
        indices.push_back(aIndex+(u16)1);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)2);
        indices.push_back(aIndex+(u16)3);
        indices.push_back(aIndex);
    }
}