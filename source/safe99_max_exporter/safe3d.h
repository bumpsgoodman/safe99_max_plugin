//************************************************************************** 
// 
// 파일: safe3d.h
// 
// 설명: safe3d 구조체 정의
// 
// 작성자: bumpsgoodman
// 
// 생성일: 2023/09/18
// 
//***************************************************************************

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct vector3
{
    float x;
    float y;
    float z;
} vector3_t;

typedef struct vector3_int
{
    int x;
    int y;
    int z;
} vector3_int_t;

typedef struct vector4
{
    float x;
    float y;
    float z;
    float w;
} vector4_t;

typedef struct safe3d
{
    char magic[8]; // "safe3d"

    DWORD num_vertices;
    DWORD num_indices;
    vector4_t wireframe_color;
} safe3d_t;