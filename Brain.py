#!/usr/bin/env python

import math

barrier = [[-3,-2],[-1,4],[6,1],[3,10],[-4,9]]

class Brain:
    def __init__(self):
        self._barrier = []
        self._table = []
        pass

    def set_barrier(self,barrier):
        self._barrier = barrier

    def print_barrier(self,barrier):
        print "Brain::print_barrier"
        for top in barrier:
            x,y = top
            print x,y
        print "End."

    def transform_to_table(self):
        for top in self._barrier:
            self._table.append(top)

        self._table.append(barrier[0])
        self.print_barrier(self._table)
    
    def get_square(self):
        xy_sum = 0;
        yx_sum = 0
        for item_index in range(0,len(self._table)-1):
            x1,y1 = self._table[item_index]
            x2,y2 = self._table[item_index+1]

            #print x1,y1,x2,y2
            xy_sum += x1*y2
            print "x1*y2",x1,y2,x1*y2
            yx_sum += y1*x2
            print "y2*x1",y2,x1

        print xy_sum,yx_sum
        total_sum =  xy_sum - yx_sum
        print "xy_sum - yx_sum",total_sum
        print "square=",total_sum/2

    def is_lines_cross(self,Pa1,Pa2,Pb1,Pb2):
        x1,y1 = Pa1
        x2,y2 = Pa2
        x3,y3 = Pb1
        x4,y4 = Pb2

        Ua = ( (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3) ) / ( (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1) )
        Ub = ( (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3) ) / ( (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1) )

        print Ua,Ub

        X = x1 + Ua*(x2-x1)
        Y = y1 + Ua*(y2-y1)

        print X,Y

        
        dx1 = x2-x1
        dy1 = y2-y1
        K1 = dx1/dy1
        print "K1=%s"%str(K1)






if __name__ == "__main__":
    brain = Brain()
    brain.set_barrier(barrier)
    brain.print_barrier(barrier)
    brain.transform_to_table()
    brain.get_square()
    brain.is_lines_cross([3,2.5],[4.5,3.5],[2.5,3.5],[5.5,3.5])
