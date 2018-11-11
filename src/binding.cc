#include <node_api.h>
#include <assert.h>
#include <igl/harmonic.h>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>

using namespace Eigen;
using namespace igl;
using namespace std;

napi_value api_harmonic(napi_env env, napi_callback_info info) {
  MatrixXd V;
  MatrixXi F;
  readOBJ("assets/bunny.obj", V, F);

  map<int, int> indices;
  for (int i = 0, n = V.rows(); i < n; i ++) {
    auto const &row = V.row(i);
    if (row.y() < 0.04) {
      indices[i] = 0;
    } else if (row.y() > 0.18) {
      indices[i] = 1;
    }
  }

  VectorXi B(indices.size());
  MatrixXd C(indices.size(), V.cols());
  int j = 0;
  for (auto i = indices.begin(), n = indices.end(); i != n; i ++, j ++) {
    B(j) = i->first;
    switch (i->second) {
      case 0:
        C.row(j) = RowVector3d(0, 0, 0);
        break;
      case 1:
        C.row(j) = RowVector3d(0, 0.1, 0.1);
        break;
    }
  }

  MatrixXd D;
  harmonic(V, F, B, C, 1, D);
  V = V + D;
  writeOBJ("assets/bunny.har.obj", V, F);

  napi_value world;
  auto status = napi_create_string_utf8(env, "world", 5, &world);
  assert(status == napi_ok);
  return world;
}

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("hello", api_harmonic);
  auto status = napi_define_properties(env, exports, 1, &desc);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
