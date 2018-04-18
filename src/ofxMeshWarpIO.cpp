#include "ofxMeshWarpIO.h"

using namespace ofx::MeshWarp;
using namespace ofx::MeshWarp::IO;

void PointHelper::get(ofBuffer &buffer) const
{
	PointData data;
	data.point = target_->point();
	data.coord = target_->coord();
	data.normal = target_->normal();
	data.color = target_->color();
	data.is_node = target_->isNode();
	buffer.append((const char*)(&data), sizeof(PointData));
}
int PointHelper::set(const char *buffer)
{
	PointData *data = (PointData*)buffer;
	target_->setPoint(data->point);
	target_->setCoord(data->coord);
	target_->setNormal(data->normal);
	target_->setColor(data->color);
	target_->setNodal(data->is_node);
	return sizeof(PointData);
}
void MeshHelper::get(ofBuffer &buffer) const
{
	MeshData data;
	data.divx = target_->getDivX();
	data.divy = target_->getDivY();
	data.resolution = target_->getChildMeshResolution();
	data.uv_rect = target_->getUVRect();
	buffer.append((const char*)(&data), sizeof(MeshData));
	auto points = target_->getPoints();
	for(auto &p : points) {
		PointHelper(p).get(buffer);
	}
}
int MeshHelper::set(const char *buffer)
{
	int ret = 0;
	MeshData *data = (MeshData*)buffer;
	target_->setup(data->divx, data->divy, 1, 1);
	target_->setUVRect(ofRectangle(data->uv_rect.x, data->uv_rect.y, data->uv_rect.width, data->uv_rect.height));
	ret += sizeof(MeshData);
	auto points = target_->getPoints();
	for(auto &p : points) {
		ret += PointHelper(p).set(buffer+ret);
	}
	return ret;
}
void Saver::addMesh(Mesh *mesh)
{
	meshes_.push_back(mesh);
}
void Saver::addMeshes(vector<Mesh*> &mesh)
{
	meshes_.insert(meshes_.end(), mesh.begin(), mesh.end());
}
void Saver::save(const string &filename) const
{
	ofBuffer buffer;
	save(buffer);
	ofBufferToFile(filename, buffer, true);
}
void Saver::save(ofBuffer &buffer) const
{
	for(auto &mesh : meshes_) {
		MeshHelper(mesh).get(buffer);
	}
}

vector<Mesh*> Loader::load(const string &filename) const
{
	return load(ofBufferFromFile(filename, true));
}

vector<Mesh*> Loader::load(const ofBuffer &buffer) const
{
	return load(buffer.getData(), buffer.size());
}

vector<Mesh*> Loader::load(const char *data, size_t size) const
{
	vector<Mesh*> ret;
	const char *end = data+size;
	while(data < end) {
		Mesh *mesh = new Mesh();
		data += MeshHelper(mesh).set(data);
		ret.push_back(mesh);
	}
	return ret;
}

