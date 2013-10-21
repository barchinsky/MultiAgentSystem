#!/usr/bin/env python

import math
import random

rad = random.randint(0,360)
vector_x = math.cos(rad)
vector_y = math.sin(rad)

print float("{0:.2f}".format(vector_x)),vector_y
