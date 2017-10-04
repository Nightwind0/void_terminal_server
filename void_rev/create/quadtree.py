#!/usr/bin/python
import random


class Square:
    def __init__(self, center, size):
        self.center = center
        self.size = size
    def containsPoint(self, point):
        if point[0] >= (self.center[0] - (self.size/2.0)) and point[0] < (self.center[0] + (self.size/2.0)) and point[1] > (self.center[1] - (self.size/2.0)) and point[1] < (self.center[1] + (self.size/2.0)):
            return True
        else:
            return False
    def intersects(self, other):
        # deal with topleft points instead of center, for my sanity
        aX1 = self.center[0] - (self.size / 2.0)
        aX2 = self.center[0] + (self.size / 2.0)
        bX1 = other.center[0] - (other.size / 2.0)
        bX2 = other.center[0] + (other.size /2.0)
        aY1 = self.center[1] - (self.size / 2.0)
        aY2 = self.center[1] + (self.size / 2.0)
        bY1 = other.center[1] - (other.size / 2.0)
        bY2 = other.center[1] + (other.size / 2.0)
        return aX1 <= bX2 and aX2 >= bX1 and aY1 <= bY2 and aY2 >= bY1



class Circle:
    def __init__(self, center, radius):
        self.center = center
        self.radius = radius

    def containsPoint(self, point):
        if math.hypot(point[0] - self.center[0], point[1] - self.center[1]) <= self.radius:
            return True
        else:
            return False
        

class Node:
    def __init__(self, square, depth, capacity):
        self.square = square
   #     print "depth = %s Square, center = %s, size = %s" % (depth, square.center, square.size)
        self.depth = depth
        self.capacity = capacity
        self.is_split = False
        self.objects = []

    def findDepth(self):
        if self.is_split:
            return max(self.topleft.findDepth(), self.topright.findDepth(), self.bottomleft.findDepth(), self.bottomright.findDepth())
        else:
            return self.depth

    def contains(self, point):
        return self.square.containsPoint(point)

    def intersects(self, other):
        return self.square.intersects(other)
    
    def bounds(self):
        return ((self.square.center[0]-self.square.size, self.square.center[1]-self.square.size), (self.square.center[0]+self.square.size, self.square.center[1]+self.square.size))

    def getCenter(self):
        return self.center
    
    def getContents(self, square):
        if not self.is_split:
            contents = self.objects[:]
            return contents
        else:
            contents = []
            if self.topleft.intersects(square):
                contents.extend(self.topleft.getContents(square))
            if self.topright.intersects(square):
                contents.extend(self.topright.getContents(square))
            if self.bottomleft.intersects(square):
                contents.extend(self.bottomleft.getContents(square))
            if self.bottomright.intersects(square):
                contents.extend(self.bottomright.getContents(square))
            return contents

    def addToChild(self, object):
        point = object.getPoint()
        if point[0] < self.square.center[0]:
            if point[1] < self.square.center[1]:
                self.topleft.addObject(object)
            else:
                self.bottomleft.addObject(object)
        else:
            if point[1] < self.square.center[1]:
                self.topright.addObject(object)
            else:
                self.bottomright.addObject(object)
                
    def addObject(self, object):
        point = object.getPoint()
#        print "Split? %s  depth = %s, object count = %s new object: %s" %(self.is_split, self.depth, len(self.objects), point)
        if not self.is_split:
            self.objects.append(object)
            if len(self.objects) > self.capacity and self.depth < 10:
                self.split()
        else:
            self.addToChild(object)

    def split(self):
        self.is_split = True
        subsize = self.square.size / 4.0
        center = self.square.center
        halfsize = self.square.size / 2.0
        topleftsquare = Square( (center[0] - subsize, center[1] - subsize), halfsize )
        toprightsquare = Square( (center[0] + subsize, center[1] - subsize), halfsize )
        bottomleftsquare = Square( (center[0] - subsize, center[1] + subsize), halfsize )
        bottomrightsquare = Square( (center[0] + subsize, center[1] + subsize), halfsize )
        self.topleft = Node(topleftsquare, self.depth + 1, self.capacity)
        self.topright = Node(toprightsquare, self.depth + 1, self.capacity)
        self.bottomleft = Node(bottomleftsquare, self.depth + 1, self.capacity)
        self.bottomright = Node(bottomrightsquare, self.depth + 1, self.capacity)

        for object in self.objects:
            self.addToChild(object)
        self.objects = None 

class Quadtree:

    def __init__(self, center, size, capacity):
        self.center = center
        self.size = size
        self.root = Node(Square(center,size), 0, capacity)

    def add(self,object):
        self.root.addObject(object)

    def depth(self):
        return self.root.findDepth()

    def findObjects(self, shape):
        fullobjects = self.root.getContents( shape )
        relevant_objects = []
        for object in fullobjects:
            if shape.containsPoint(object.getPoint()):
                relevant_objects.append(object)
        return relevant_objects

class Thing:
    def __init__(self, point):
        self.point = point
    def getPoint(self):
        return self.point
    
tree_size = 1000
    
tree = Quadtree( (0,0) , 1000,  20)

for i in range (0, 100000):
    tree.add(Thing((random.randint(- tree_size / 2, tree_size /2), random.randint(- tree_size / 2, tree_size /2))))

print "depth is %s" % (tree.depth())
thingies = tree.findObjects( Square((0,0), 5.0))

count = 0
for thingy in thingies:
    count = count + 1
    print "%s %s" % (count, thingy.getPoint())
