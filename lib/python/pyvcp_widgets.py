from Tkinter import *
import math
#import timer

class pyvcp_meter(Canvas):
    """ Meter - shows the value of a FLOAT with an analog meter
        <meter>
            [ <size>300</size> ]
            [ <halpin>"mymeter"</halpin> ]
            [ <text>"My Voltage"</text> ]
            [ <subtext>"Volts"</subtext>
            [ <min_>-22</min_> ]
            [ <max_>123</max_> ]
            [ <majorscale>10</majorscale> ]
            [ <minorscale>5</minorscale> ]
            [ <region1>(70,80,"green")</region1> ]
            [ <region2>(80,100,"orange")</region2> ]
            [ <region3>(100,123,"red")</region3> ]
        </meter>
    """
    # FIXME: logarithmic scale option
    n=0
    def __init__(self,root,pycomp,halpin=None, size=200,text=None,subtext=None,min_=0,max_=100,majorscale=None, minorscale=None,region1=None,region2=None,region3=None,**kw):
        self.size = size
        self.pad=10
        Canvas.__init__(self,root,width=size,height=size)
        self.halpin=halpin
        self.min_=min_
        self.max_=max_
        range_=2.5
        self.min_alfa=-math.pi/2-range_
        self.max_alfa=-math.pi/2+range_
        self.circle=self.create_oval(self.pad,self.pad,size-self.pad,size-self.pad, width=2)
        self.itemconfig(self.circle,fill="white")
        self.mid=size/2
        self.r=(size-2*self.pad)/2
        self.alfa=0
        if minorscale==None:
            self.minorscale=0
        else:
            self.minorscale=minorscale
        if majorscale==None:
            self.majorscale=float((self.max_-self.min_)/10)
        else:
            self.majorscale=majorscale
        if text!=None: t=self.create_text([self.mid,self.mid-size/12],font="Arial %d bold" % (size/10),text=text)
        if subtext!=None: t=self.create_text([self.mid,self.mid+size/12],font="Arial %d" % (size/30+5),text=subtext)
        if region1!=None: self.draw_region(region1)
        if region2!=None: self.draw_region(region2)
        if region3!=None: self.draw_region(region3)
        self.draw_ticks()

        self.line = self.create_line([self.mid,self.mid, self.mid+self.r*math.cos(self.alfa), self.mid+self.r*math.sin(self.alfa)],fill="red", arrow="last", arrowshape=(0.9*self.r,self.r,self.r/20))
        self.itemconfig(self.line,width=3)

        # create the hal pin
        if halpin == None:
            self.halpin = "meter."+str(pyvcp_meter.n)+".value"
            pyvcp_meter.n += 1
        pycomp.newpin(self.halpin, HAL_FLOAT, HAL_IN)
        self.value = pycomp[self.halpin]

    def rad2deg(self, rad): return rad*180/math.pi

    def value2angle(self, value):
            #returns angle for a given value
            scale = (self.max_-self.min_)/(self.max_alfa-self.min_alfa)
            alfa = self.min_alfa + (value-self.min_)/scale
            if alfa > self.max_alfa:
                alfa = self.max_alfa
            elif alfa < self.min_alfa:
                alfa = self.min_alfa
            return alfa

    def p2c(self, radius, angle):
        #returns the cathesian coordinates (x,y) for given polar coordinates
        #radius in percent of self.r; angle in radians
        return self.mid+radius*self.r*math.cos(angle), self.mid+radius*self.r*math.sin(angle)

    def update(self,pycomp):
        self.value = pycomp[self.halpin]
        self.alfa = self.value2angle(self.value)
        x,y = self.p2c(0.8, self.alfa)
        self.coords(self.line,self.mid,self.mid,x,y)

    def draw_region(self, (start, end, color)):
            #Draws a colored region on the canvas between start and end
            start = self.value2angle(start)
            start = -self.rad2deg(start)
            end = self.value2angle(end)
            end = -self.rad2deg(end)
            extent = end-start
            halfwidth = math.floor(0.1*self.r/2)+1
            xy = self.pad+halfwidth, self.pad+halfwidth, self.size-self.pad-halfwidth, self.size-self.pad-halfwidth
            self.create_arc(xy, start=start, extent=extent, outline=color, width=(halfwidth-1)*2, style="arc")

    def draw_ticks(self):
        value = self.min_
        while value <= self.max_:
            alfa = self.value2angle(value)
            xy1 = self.p2c(1,alfa)
            xy2 = self.p2c(0.85,alfa)
            xytext = self.p2c(0.75,alfa)
            self.create_text(xytext,font="Arial %d" % (self.size/30+5), text="%g" % value)
            self.create_line(xy1, xy2, width=2)
            value = value + self.majorscale
        #minor ticks
        value = self.min_
        if self.minorscale > 0:
            while value <= self.max_:
                if (value % self.majorscale) != 0:
                    alfa = self.value2angle(value)
                    xy1 = self.p2c(1,alfa)
                    xy2 = self.p2c(0.9,alfa)
                    self.create_line(xy1, xy2)
                value = value + self.minorscale



