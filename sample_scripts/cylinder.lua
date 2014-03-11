setCamera({0,-50,30},{0,1,0},{1,0,0},{0,0,1})
setLight({50,-50,50},makeColor(1,1,1))

putPlane({0,0,0},{20,0,0},{0,20,0},nil,nil,nil,3)
setPrevObjectTexture("sirokuro.png",1,0.4,1)
putCylinder({0,0,0},{0,0,1},0,20,10)
setPrevObjectColor(makeColor(0,1,0),0.2,0.1,1)

putDisk({0,0,20},{10,0,0},{0,10,0})
setPrevObjectColor(makeColor(0.5,1,0.5),1,0.4,1.2)
