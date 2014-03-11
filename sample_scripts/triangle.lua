cangle=-math.pi/6

setCamera(
	{0,0,0},
	vectorRotateYZ(cangle,{0,1,0}),
	{1,0,0},
	vectorRotateYZ(cangle,{0,0,1}))
setLight({50,0,50},makeColor(1,1,1))

putTriangle({-30,30,-30},{20,0,0},{0,20,0},2,2)
setPrevObjectTexture("sirokuro.png",1,0.8,0.5)

putTriangle({40,100,-30},{-20,0,0},{0,-20,0},2,2)
setPrevObjectColor(makeColorFromInt(255,214,0),1,0.8,0.5)

putPlane({0,0,-50},{1,0,0},{0,1,0},nil,nil,nil,nil)
setPrevObjectColor(makeColor(1,1,1),0.7,0.8,0.8)
