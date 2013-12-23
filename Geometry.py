#!/usr/bin/env python
import math

class Point:	
	def __init__(self, x, y):
		self.additional_info = {}
		self.x = float(x)
		self.y = float(y)

	def add(self,point2):
		point = Point(self.x,self.y)
		point.x += point2.x
		point.y += point2.y
		return point

	def move_in_direction_with_length(self,direction,length = 1):		
		x = self.x + length * math.cos(direction)
		y = self.y + length * math.sin(direction) 
		return Point(x,y)


	def sub(self,point2):
		point = Point(self.x,self.y)
		point.x -= point2.x
		point.y -= point2.y
		return point

	def equal(self,point):
		EPS = 1e-9
		return ((point.x - EPS <= self.x <= point.x + EPS) and (point.y - EPS <= self.y <= point.y + EPS))

	def getArr(self):
		return [self.x, self.y]

	def log(self):
		print "Point ",self," (",self.x,",",self.y,")"
		for key in self.additional_info.keys():
			print key,": ",self.additional_info[key]


class Line:	
	def __init__(self, p1, p2):		
		self.p1 = p1
		self.p2 = p2

	def is_containt_point(self,p):		
		x1 = self.p1.x
		y1 = self.p1.y
		x2 = self.p2.x
		y2 = self.p2.y
		EPS = 1e-9
		check_x = min(x1,x2) - EPS<=p.x<=max(x1,x2) + EPS
		check_y = min(y1,y2) - EPS<=p.y<=max(y1,y2) + EPS		
		contain = check_x and check_y
		return contain	

	def log(self):
		print "Line log:\n"
		self.p1.log()
		self.p2.log()

	def getVector(self):
		return Vector(self.p1,self.p2)

	def getAngle(self): # in rad
		angle = self.getVector().getAngle()
		return angle		

	def length(self):
		return self.getVector().length()

	def height_to_point(self,point):
		a = self.length()
		b = Line(self.p1,point).length()
		c = Line(self.p2,point).length()
		p = 0.5 * (a + b + c)
		ha = 2 / a * (p*(p-a)*(p-b)*(p-c))**0.5
		return ha

	def point_of_height_vertex_to_right_direction(self,height,is_right_dir):		
		vec = self.getVector()
		length = vec.length()
		pi = math.pi

		angle = vec.getAngle();		
		if is_right_dir:
			angle -= pi / 2
		else:
			angle += pi / 2

		angle = angle % (2*pi)
		x_dir, y_dir = 1, 1

		if angle > pi / 2 and angle < pi * 1.5:
			x_dir = -1
		if angle > pi and angle < pi * 2:
			y_dir = -1

		hx = self.p1.x + abs(vec.y) / length * x_dir * height
		hy = self.p1.y + abs(vec.x) / length * y_dir * height 
		return Point(hx,hy)

class Vector:		
	def __init__(self, p1 = None, p2 = None):		
		if p1 and p2:
			coord = p2.sub(p1)
			self.x = coord.x
			self.y = coord.y
		else:
			self.x = 0
			self.y = 0

	def setAngleAndLength(self,radian, length = 1):
		self.x = length * math.cos(radian)
		self.y = length * math.sin(radian)

	def length(self):
		x = self.x
		y = self.y
		return (x**2 + y**2)**0.5

	def getAngle(self):
		angle = math.atan2(self.y,self.x)
		if angle < 0:
				angle = math.pi*2 + angle 
		return angle

	def getArr(self):
		return [self.x, self.y]


class LiniarFunction:	
	def __init__(self, line = None):
		if line:
			self.setLine(line)

	def setLine(self,line):
		self.line = line
		self.__calculate_function()

	def setPointAndAngle(self,point,angle):
		x2 = point.x + math.cos(angle)
		y2 = point.y + math.sin(angle)
		self.line = Line(point,Point(x2,y2))
		self.__calculate_function()
		

	def __calculate_function(self):
		x1 = self.line.p1.x
		y1 = self.line.p1.y
		x2 = self.line.p2.x
		y2 = self.line.p2.y

		self.A = y1 - y2
		self.B = x2 - x1
		self.C = x1*y2 - x2*y1


	def __det(self,A,B,C,D):
		return A*D - B*C		

	def intersect(self,line_fun):
		zn = self.__det(self.A, self.B, line_fun.A, line_fun.B)
		EPS = 1e-9
		if math.fabs(zn) < EPS:
			return False, None

		x = -self.__det(self.C, self.B, line_fun.C, line_fun.B) / zn
		y = -self.__det(self.A, self.C, line_fun.A, line_fun.C) / zn
		return True, Point(x,y)

	def intersect_with_segment(self,segment):
		is_intersect, point = self.intersect(LiniarFunction(segment))

		if is_intersect:			
			if segment.is_containt_point(point):
				return True, point
		
		return False, None		

	def parallel(self, line_fun):
		EPS = 1e-9
		is_parallel = math.fabs(self.__det(self.A, self.B, self.A, self.B)) < EPS 
		return is_parallel

	def equivalent(self, line_fun):
		EPS = 1e-9
		A1, A2 = self.A, line_fun.A
		B1, B2 = self.B, line_fun.B
		C1, C2 = self.C, line_fun.C

		is_equivalent = (math.fabs(self.__det(A1,B1,A2,B2)) < EPS and
				math.fabs(self.__det(A1,C1,A2,C2)) < EPS and
				math.fabs(self.__det(B1,C1,B2,C2)) < EPS)
		return is_equivalent




class Polygon:	
	def __init__(self, list_of_coords):		
		self.points = []
		for coords in list_of_coords:
			self.points.append(Point(coords[0],coords[1]))		

	def addPoint(self,point):
		self.points.append(point)

	def isPolygon(self):
		return len(self.points) > 2

	def log(self):
		for point in self.points:
			point.log()

	def isRayIntersect(self,input_point,angle_in_rad):			
		angles = []
		for point in self.points:
			line = Line(input_point,point)
			angle = line.getAngle()
			if angle < 0:
				angle = math.pi*2 + angle 
			angles.append(angle)			

		if angle_in_rad >= min(angles) and angle_in_rad <= max(angles):
			return True

		return False

	def getSmallerShapeDevidedByFundtion(self,liniar_function):

		base_point = liniar_function.line.p1		
		start_angle = liniar_function.line.getAngle()

		if not self.isRayIntersect(base_point,start_angle):
			return {"is_intersect": False}


		left_shape = Polygon([])
		right_shape = Polygon([])		 

		count = len(self.points)		
		intersect_points = []
		for i in range(count):
			p1 = self.points[i]
			p2 = self.points[i+1] if (i + 1) < count else self.points[0]

			#add already existed point to shape		
			angle = Line(base_point,p1).getAngle()
			start_angle_plus_pi = start_angle + math.pi

			if angle == start_angle:				
				left_shape.addPoint(p1)
				right_shape.addPoint(p1)
			else:
				if start_angle < angle and angle <= start_angle_plus_pi:					
					left_shape.addPoint(p1)
				else:					
					right_shape.addPoint(p1)


			line = Line(p1,p2)			
			is_intersect, point = liniar_function.intersect_with_segment(line)

			# added intersect point	
			if is_intersect:				
				point.additional_info["marked"] = True
				point_added = False
				
				if len(intersect_points) == 0:
					intersect_points.append(point)
					point_added = True
				else:
					p_last = intersect_points[len(intersect_points) - 1]
					p_first = intersect_points[0]
					if not p_last.equal(point) and not p_first.equal(point):
						intersect_points.append(point)
						point_added = True

				if point_added:
					left_shape.addPoint(point)
					right_shape.addPoint(point)

		
		if len(intersect_points) == 0:
			return {"is_intersect": False}
		elif len(intersect_points) == 1:
			return {"is_intersect": True, "one_vertex":intersect_points[0]}
		
		left_perimentr = left_shape.get_perimeter_length()		
		right_perimentr = right_shape.get_perimeter_length()
		
		is_left_shape = left_perimentr < right_perimentr
		# print "\n\n+++++++++++++++++++++++++++++\nTEST IS LEFT SHAPE ",is_left_shape
		# print "\nleft shape:\n"
		# left_shape.log()
		# print "\nright shape:\n"
		# right_shape.log()
		# print "\n++++++++++++++++++++++++++++++++\n\n"

		smaller_shape = left_shape if is_left_shape else right_shape

		min_inter_point = Point(0,0)
		max_inter_point = Point(0,0)
		min_length = 99999999
		max_length = 0

		for point in intersect_points:
			line = Line(base_point,point)
			tmp_length = line.length()
			if tmp_length < min_length:
				min_length = tmp_length
				min_inter_point = point
			if tmp_length > max_length:
				max_length = tmp_length
				max_inter_point = point

		#mark min and max points in shape
		for point in smaller_shape.points:
			if min_inter_point.equal(point):
				point.additional_info["min"] = True
			elif max_inter_point.equal(point):
				point.additional_info["max"] = True
		
		smaller_shape = self.__sort_marked_shape(smaller_shape,is_left_shape)
		
		return {"is_intersect": True,"is_left_shape": is_left_shape, "shape":smaller_shape}

	def get_perimeter_length(self):
		common_length = 0
		for item_index in range(0,len(self.points)-1):
			p1 = self.points[item_index]
			p2 = self.points[item_index+1]
			line = Line(p1,p2)
			common_length += line.length()
		return common_length


	def __sort_marked_shape(self,shape,is_left_direction):
		sorted_shape = Polygon([])

		first_point_index = 0
		last_point_index = 0

		i = 0
		for point in shape.points:		
			if point.additional_info.has_key("min") and point.additional_info["min"] == True:
				first_point_index = i
			elif point.additional_info.has_key("max") and point.additional_info["max"] == True:
				last_point_index = i
			i += 1

		#print "\ncheck point 1\n"
		#print "first_point_index ",first_point_index
		#print "last_point_index ",last_point_index

		#select direction
		left_index = first_point_index - 1 if first_point_index != 0 else len(shape.points) - 1
		right_index = first_point_index + 1 if first_point_index != len(shape.points) - 1 else 0 

		#print "left_index ",left_index
		#print "right_index ",right_index

		direction = 0
		if not shape.points[left_index].additional_info.has_key("marked") and shape.points[right_index].additional_info.has_key("marked"):
			direction = -1
		elif not shape.points[right_index].additional_info.has_key("marked") and shape.points[left_index].additional_info.has_key("marked"):
			direction = 1

		if direction == 0:
			direction = -1 if is_left_direction else 1				

		#print "\ncheck point 2\n"

		i = first_point_index

		while 1:
			new_point = shape.points[i]

			count = len(sorted_shape.points)
			new_equal_to_old = new_point.equal(sorted_shape.points[-1]) if count > 0 else False

			if not new_equal_to_old:
				sorted_shape.addPoint(new_point)		

			if i == last_point_index:
				break

			i += direction
			if i < 0:
				i = len(shape.points) - 1
			elif i > (len(shape.points) - 1):
				i = 0

		return sorted_shape


#-------------------------------------------------------
#---   	    			TESTS 
#-------------------------------------------------------

def angle_test(parts,x0,y0):
	#quick angle test

	rad_in_step = math.pi / parts	
	p0 = Point(x0,y0)
	radius = 10
	print "rad step ", rad_in_step 
	
	for step in range(0,parts):
		rad = rad_in_step * step
		x = x0 + radius * math.cos(rad)
		y = y0 + radius * math.sin(rad)
		print "\n\nx: ",x,"y: ",y
		print "should be angle: ",rad
		
		line = Line(p0,Point(x,y))
		angle = line.getAngle()
		print "result anlge: ",angle
		print "diff ",rad - angle



def sort_shape_test():
	# points = [[-2,0],[0,-2],[4,-1],[1,0],[5,2],[0,5],[-2,3]]

	points = [[0.2,0.2],[0.6,0.2],[0.6,0.6],[0.2,0.6]]
	shape = Polygon(points)

	
	#for i in range(89,92):
	deg = 80
	rad = deg * math.pi / 180
	print "\n\n------------------------\nTEST FOR ",deg," DEGREE\n"

	x0 ,y0 = 0.199448, 0.454525
	#x0 ,y0 = 0.15, 0.15

	x = x0 + math.cos(deg*math.pi/180)
	y = y0 + math.sin(deg*math.pi/180)
	p1 = Point(x0,y0)
	p2 = Point(x,y)
	line = Line(p1,p2) 
		
	lin_fun = LiniarFunction(line)

	selfLinFunc = LiniarFunction()
	selfLinFunc.setPointAndAngle(Point(x0,y0),rad)


	inter_dict = shape.getSmallerShapeDevidedByFundtion(selfLinFunc)
	
	if inter_dict["is_intersect"]:
		if inter_dict.has_key("one_vertex"):
			p = inter_dict["one_vertex"]
			p.log()
		if inter_dict.has_key("shape"):	
			print "\n\n-----------------\nIs left Shape: ",inter_dict["is_left_shape"]						
			inter_dict["shape"].log()
	else:
		print "Don't intersect"

def test_height():
	p1 = Point(0,0)
	p2 = Point(5,0)
	p3 = Point(2,-2)
	
	line = Line(p1,p2)
	h = line.height_to_point(p3)
	print "height ",h

	line.log()

	hp = line.point_of_height_vertex_to_right_direction(h,False)
	hp.log()



if __name__=='__main__':
	#angle_test(4,0,0)
	#square_test()
	sort_shape_test()
	#test_height()