template<typename T>
vector3d grid3d<T>::getCoords(const T *item) {
	int index = (int)(item - data);

    int z = index % _depth;
    int index2d = index / _depth;
    int x = index2d % w;
    int y = index2d / w;

    vector3d vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

template<typename T>
void grid3d<T>::resetGrid(int newW, int newH, int newDepth) {
    w = newW;
    h = newH;
    _depth = newDepth;
    delete[]data;
    data = new T[newW * newH * newDepth];
}
