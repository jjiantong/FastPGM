//
// Created by jjt on 17/09/22.
//

#ifndef BAYESIANNETWORK_COMMON_H
#define BAYESIANNETWORK_COMMON_H

#define SAFE_DELETE(p) do { if(p) { delete (p); (p) = nullptr; } } while(0)
#define SAFE_DELETE_ARRAY(p) do { if(p) { delete[] (p); (p) = nullptr; } } while(0)

#endif //BAYESIANNETWORK_COMMON_H
