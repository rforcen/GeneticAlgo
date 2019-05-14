//
//  dummy.metal
//  GeneticAlgos
//
//  Created by asd on 14/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


kernel void dummy(device float*data[[buffer(0)]],
                  uint2  pos [[thread_position_in_grid]]
                  )
{
    data[pos.x]=0;
}
