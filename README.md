3D画像生成
----------

### 何をするソフト？

カメラ、ライト、オブジェクトの位置を指定することで、
きれいな3次元画像を生成する。

### 特徴は？
- Luaのスクリプトで世界を表すので、オブジェクトの位置を簡単に決められる。
- 鏡面反射を考えたきれいな画像が作れる。

### 欠点は？
- 基本的に、単純な立体(平面、球、円柱)しか扱えない。
- 生成に長時間かかる。

### 関連項目は？
- [前段階として開発された、球の画像を生成するプログラム](https://gist.github.com/mikecat/9486977)
- [それをDXライブラリのシェーダーに移植したもの](http://dixq.net/forum/blog.php?u=536&b=4539)

### ライセンスは？
別に指定が無ければ、The zlib/libpng Licenseです。

Copyright (c) 2014 みけCAT

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented;
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the
   product documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such,
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
