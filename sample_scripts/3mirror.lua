if getCommandParamNum()>=1 then
	setTtl(getCommandParam(1))
else
	setTtl(32)
end

setCamera({0,-10,25},{0,1,0},{1,0,0},{0,0,1})
setLight({0,0,50},makeColor(1,1,1))
putPlane({0,0,0},{20,0,0},{0,20,0},nil,nil,nil,nil)
setPrevObjectTexture("sirokuro.png",0.8,0.7,0.8)

vertex1={0,50,0}
vertex2=vectorRotateXY(-math.pi*2.0/3.0,vertex1)
vertex3=vectorRotateXY(math.pi*2.0/3.0,vertex1)

mirrorHeight=nil

putPlane(vertex1,vectorSub(vertex2,vertex1),{0,0,1},0,1,0,mirrorHeight)
setPrevObjectColor(makeColor(0,0,0),0.01,0,1)
putPlane(vertex2,vectorSub(vertex3,vertex2),{0,0,1},0,1,0,mirrorHeight)
setPrevObjectColor(makeColor(0,0,0),0.01,0,1)
putPlane(vertex3,vectorSub(vertex1,vertex3),{0,0,1},0,1,0,mirrorHeight)
setPrevObjectColor(makeColor(0,0,0),0.01,0,1)

putCylinder(vertex1,vectorSub(vertex2,vertex1),0,1,0.5)
setPrevObjectColor(makeColor(0,0,0),1,1,0.6)
putCylinder(vertex2,vectorSub(vertex3,vertex2),0,1,0.5)
setPrevObjectColor(makeColor(0,0,0),1,1,0.6)
putCylinder(vertex3,vectorSub(vertex1,vertex3),0,1,0.5)
setPrevObjectColor(makeColor(0,0,0),1,1,0.6)

putSphere({0,15,15},5)
setPrevObjectColor(makeColor(0,1,0),0.4,0.5,1.2)
