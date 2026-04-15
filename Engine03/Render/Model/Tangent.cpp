//
//  Tangent.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/4/15.
//

#include "Tangent.hpp"
#include "mikktspace.h"


namespace BTflag {
namespace Model {

int m_getNumFaces(const SMikkTSpaceContext * pContext) {
    Model *pModel = static_cast<Model *>( pContext->m_pUserData );
    
    return static_cast<int>( pModel->vertexIndices.size() / 3 );
}

int m_getNumVerticesOfFace(const SMikkTSpaceContext * pContext, const int iFace) {
    return 3;
}

void m_getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert) {
    Model *pModel = static_cast<Model *>( pContext->m_pUserData );
    uint32_t index = pModel->vertexIndices[iFace * 3 + iVert];
    memcpy(fvPosOut, &pModel->position[index], 12);
}

void m_getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert) {
    Model *pModel = static_cast<Model *>( pContext->m_pUserData );
    uint32_t index = pModel->vertexIndices[iFace * 3 + iVert];
    memcpy(fvNormOut, &pModel->normal[index], 12);
}

void m_getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert) {
    Model *pModel = static_cast<Model *>( pContext->m_pUserData );
    uint32_t index = pModel->vertexIndices[iFace * 3 + iVert];
    memcpy(fvTexcOut, &pModel->uv1[index], 8);
}

void m_setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert) {
    Model *pModel = static_cast<Model *>( pContext->m_pUserData );
    uint32_t index = pModel->vertexIndices[iFace * 3 + iVert];
    pModel->tangent[index] = simd::make_float4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
}

void TangentGen(Model *model) {
    model->tangent.resize(model->position.size());
    
    SMikkTSpaceInterface iface = {};
    iface.m_getNumFaces = m_getNumFaces;
    iface.m_getNumVerticesOfFace = m_getNumVerticesOfFace;
    iface.m_getPosition = m_getPosition;
    iface.m_getNormal = m_getNormal;
    iface.m_getTexCoord = m_getTexCoord;
    iface.m_setTSpaceBasic = m_setTSpaceBasic;
    
    SMikkTSpaceContext ctx = {&iface, model};
    genTangSpaceDefault(&ctx);
}


};
}
