##  ======== Vector Types ========

type
  Vec2f* {.bycopy.} = object
    x*: cfloat
    y*: cfloat

  Vec3f* {.bycopy.} = object
    x*: cfloat
    y*: cfloat
    z*: cfloat

  Vec4f* {.bycopy.} = object
    x*: cfloat
    y*: cfloat
    z*: cfloat
    w*: cfloat

  Vec2i* {.bycopy.} = object
    x*: cint
    y*: cint

  Vec3i* {.bycopy.} = object
    x*: cint
    y*: cint
    z*: cint

  Vec4i* {.bycopy.} = object
    x*: cint
    y*: cint
    z*: cint
    w*: cint


##  ======== Matrix Types ========

type
  Mat4f* {.bycopy.} = object
    m*: array[4, array[4, cfloat]]

  Mat3f* {.bycopy.} = object
    m*: array[3, array[3, cfloat]]


##  ======== Quaternion Type ========

type
  Quatf* {.bycopy.} = object
    x*: cfloat
    y*: cfloat
    z*: cfloat
    w*: cfloat


##  ======== Vec3f Operations ========

proc vec3f_add*(a: Vec3f; b: Vec3f): Vec3f {.inline.} =
  ## !!!Ignored construct:  return ( Vec3f ) { a . x + b . x , a . y + b . y , a . z + b . z } ;
  ## Error: token expected: ; but got: {!!!

proc vec3f_sub*(a: Vec3f; b: Vec3f): Vec3f {.inline.} =
  ## !!!Ignored construct:  return ( Vec3f ) { a . x - b . x , a . y - b . y , a . z - b . z } ;
  ## Error: token expected: ; but got: {!!!

proc vec3f_scale*(v: Vec3f; s: cfloat): Vec3f {.inline.} =
  ## !!!Ignored construct:  return ( Vec3f ) { v . x * s , v . y * s , v . z * s } ;
  ## Error: token expected: ; but got: {!!!

proc vec3f_dot*(a: Vec3f; b: Vec3f): cfloat {.inline.} =
  return a.x * b.x + a.y * b.y + a.z * b.z

proc vec3f_cross*(a: Vec3f; b: Vec3f): Vec3f {.inline.} =
  ## !!!Ignored construct:  return ( Vec3f ) { a . y * b . z - a . z * b . y , a . z * b . x - a . x * b . z , a . x * b . y - a . y * b . x } ;
  ## Error: token expected: ; but got: {!!!

proc vec3f_length*(v: Vec3f): cfloat {.inline.} =
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z)

proc vec3f_normalize*(v: Vec3f): Vec3f {.inline.} =
  var len: cfloat = vec3f_length(v)
  return if len > 0.0f: vec3f_scale(v, 1.0f div len) else: v

##  ======== Vec4f Operations ========

proc vec4f_add*(a: Vec4f; b: Vec4f): Vec4f {.inline.} =
  ## !!!Ignored construct:  return ( Vec4f ) { a . x + b . x , a . y + b . y , a . z + b . z , a . w + b . w } ;
  ## Error: token expected: ; but got: {!!!

proc vec4f_sub*(a: Vec4f; b: Vec4f): Vec4f {.inline.} =
  ## !!!Ignored construct:  return ( Vec4f ) { a . x - b . x , a . y - b . y , a . z - b . z , a . w - b . w } ;
  ## Error: token expected: ; but got: {!!!

proc vec4f_scale*(v: Vec4f; s: cfloat): Vec4f {.inline.} =
  ## !!!Ignored construct:  return ( Vec4f ) { v . x * s , v . y * s , v . z * s , v . w * s } ;
  ## Error: token expected: ; but got: {!!!

proc vec4f_dot*(a: Vec4f; b: Vec4f): cfloat {.inline.} =
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w

proc vec4f_length*(v: Vec4f): cfloat {.inline.} =
  return sqrtf(vec4f_dot(v, v))

proc vec4f_normalize*(v: Vec4f): Vec4f {.inline.} =
  var len: cfloat = vec4f_length(v)
  return if len > 0.0f: vec4f_scale(v, 1.0f div len) else: v

##  ======== Integer Vectors ========

proc vec3i_add*(a: Vec3i; b: Vec3i): Vec3i {.inline.} =
  ## !!!Ignored construct:  return ( Vec3i ) { a . x + b . x , a . y + b . y , a . z + b . z } ;
  ## Error: token expected: ; but got: {!!!

proc vec3i_sub*(a: Vec3i; b: Vec3i): Vec3i {.inline.} =
  ## !!!Ignored construct:  return ( Vec3i ) { a . x - b . x , a . y - b . y , a . z - b . z } ;
  ## Error: token expected: ; but got: {!!!

proc vec4i_add*(a: Vec4i; b: Vec4i): Vec4i {.inline.} =
  ## !!!Ignored construct:  return ( Vec4i ) { a . x + b . x , a . y + b . y , a . z + b . z , a . w + b . w } ;
  ## Error: token expected: ; but got: {!!!

proc vec4i_sub*(a: Vec4i; b: Vec4i): Vec4i {.inline.} =
  ## !!!Ignored construct:  return ( Vec4i ) { a . x - b . x , a . y - b . y , a . z - b . z , a . w - b . w } ;
  ## Error: token expected: ; but got: {!!!

##  ======== Mat4f Operations ========

proc mat4f_identity*(): Mat4f {.inline.} =
  var m: Mat4f = Mat4f(m: 0)
  m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f
  return m

proc mat4f_mul*(a: Mat4f; b: Mat4f): Mat4f {.inline.} =
  var r: Mat4f = Mat4f(m: 0)
  var i: cint = 0
  while i < 4:
    var j: cint = 0
    while j < 4:
      var k: cint = 0
      while k < 4:
        inc(r.m[i][j], a.m[i][k] * b.m[k][j])
        inc(k)
      inc(j)
    inc(i)
  return r

proc mat4f_mul_vec4f*(m: Mat4f; v: Vec4f): Vec4f {.inline.} =
  ## !!!Ignored construct:  return ( Vec4f ) { m . m [ 0 ] [ 0 ] * v . x + m . m [ 0 ] [ 1 ] * v . y + m . m [ 0 ] [ 2 ] * v . z + m . m [ 0 ] [ 3 ] * v . w , m . m [ 1 ] [ 0 ] * v . x + m . m [ 1 ] [ 1 ] * v . y + m . m [ 1 ] [ 2 ] * v . z + m . m [ 1 ] [ 3 ] * v . w , m . m [ 2 ] [ 0 ] * v . x + m . m [ 2 ] [ 1 ] * v . y + m . m [ 2 ] [ 2 ] * v . z + m . m [ 2 ] [ 3 ] * v . w , m . m [ 3 ] [ 0 ] * v . x + m . m [ 3 ] [ 1 ] * v . y + m . m [ 3 ] [ 2 ] * v . z + m . m [ 3 ] [ 3 ] * v . w } ;
  ## Error: token expected: ; but got: {!!!

proc mat4f_translate*(x: cfloat; y: cfloat; z: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  m.m[0][3] = x
  m.m[1][3] = y
  m.m[2][3] = z
  return m

proc mat4f_scale_mat*(x: cfloat; y: cfloat; z: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  m.m[0][0] = x
  m.m[1][1] = y
  m.m[2][2] = z
  return m

proc mat4f_rotate_x*(angle: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  var
    c: cfloat = cosf(angle)
    s: cfloat = sinf(angle)
  m.m[1][1] = c
  m.m[1][2] = -s
  m.m[2][1] = s
  m.m[2][2] = c
  return m

proc mat4f_rotate_y*(angle: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  var
    c: cfloat = cosf(angle)
    s: cfloat = sinf(angle)
  m.m[0][0] = c
  m.m[0][2] = s
  m.m[2][0] = -s
  m.m[2][2] = c
  return m

proc mat4f_rotate_z*(angle: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  var
    c: cfloat = cosf(angle)
    s: cfloat = sinf(angle)
  m.m[0][0] = c
  m.m[0][1] = -s
  m.m[1][0] = s
  m.m[1][1] = c
  return m

##  ======== Mat3f Operations ========

proc mat3f_identity*(): Mat3f {.inline.} =
  var m: Mat3f = Mat3f(m: 0)
  m.m[0][0] = m.m[1][1] = m.m[2][2] = 1.0f
  return m

proc mat3f_mul_vec3f*(m: Mat3f; v: Vec3f): Vec3f {.inline.} =
  ## !!!Ignored construct:  return ( Vec3f ) { m . m [ 0 ] [ 0 ] * v . x + m . m [ 0 ] [ 1 ] * v . y + m . m [ 0 ] [ 2 ] * v . z , m . m [ 1 ] [ 0 ] * v . x + m . m [ 1 ] [ 1 ] * v . y + m . m [ 1 ] [ 2 ] * v . z , m . m [ 2 ] [ 0 ] * v . x + m . m [ 2 ] [ 1 ] * v . y + m . m [ 2 ] [ 2 ] * v . z } ;
  ## Error: token expected: ; but got: {!!!

##  ======== Quaternion Operations ========

proc quat_identity*(): Quatf {.inline.} =
  ## !!!Ignored construct:  return ( Quatf ) { 0 , 0 , 0 , 1 } ;
  ## Error: token expected: ; but got: {!!!

proc quat_from_axis_angle*(axis: Vec3f; angle: cfloat): Quatf {.inline.} =
  var half: cfloat = angle * 0.5f
  var s: cfloat = sinf(half)
  axis = vec3f_normalize(axis)
  ## !!!Ignored construct:  return ( Quatf ) { axis . x * s , axis . y * s , axis . z * s , cosf ( half ) } ;
  ## Error: token expected: ; but got: {!!!

proc quat_mul*(a: Quatf; b: Quatf): Quatf {.inline.} =
  ## !!!Ignored construct:  return ( Quatf ) { a . w * b . x + a . x * b . w + a . y * b . z - a . z * b . y , a . w * b . y - a . x * b . z + a . y * b . w + a . z * b . x , a . w * b . z + a . x * b . y - a . y * b . x + a . z * b . w , a . w * b . w - a . x * b . x - a . y * b . y - a . z * b . z } ;
  ## Error: token expected: ; but got: {!!!

proc quat_to_mat4f*(q: Quatf): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  var
    xx: cfloat = q.x * q.x
    yy: cfloat = q.y * q.y
    zz: cfloat = q.z * q.z
  var
    xy: cfloat = q.x * q.y
    xz: cfloat = q.x * q.z
    yz: cfloat = q.y * q.z
  var
    wx: cfloat = q.w * q.x
    wy: cfloat = q.w * q.y
    wz: cfloat = q.w * q.z
  m.m[0][0] = 1 - 2 * (yy + zz)
  m.m[0][1] = 2 * (xy - wz)
  m.m[0][2] = 2 * (xz + wy)
  m.m[1][0] = 2 * (xy + wz)
  m.m[1][1] = 1 - 2 * (xx + zz)
  m.m[1][2] = 2 * (yz - wx)
  m.m[2][0] = 2 * (xz - wy)
  m.m[2][1] = 2 * (yz + wx)
  m.m[2][2] = 1 - 2 * (xx + yy)
  return m

##  ======== Projection Matrices ========

proc mat4f_perspective*(fovy: cfloat; aspect: cfloat; nearZ: cfloat; farZ: cfloat): Mat4f {.
    inline.} =
  var m: Mat4f = Mat4f(m: 0)
  var f: cfloat = 1.0f div tanf(fovy * 0.5f)
  m.m[0][0] = f div aspect
  m.m[1][1] = f
  m.m[2][2] = (farZ + nearZ) div (nearZ - farZ)
  m.m[2][3] = (2 * farZ * nearZ) div (nearZ - farZ)
  m.m[3][2] = -1
  return m

proc mat4f_orthographic*(left: cfloat; right: cfloat; bottom: cfloat; top: cfloat;
                        nearZ: cfloat; farZ: cfloat): Mat4f {.inline.} =
  var m: Mat4f = mat4f_identity()
  m.m[0][0] = 2.0f div (right - left)
  m.m[1][1] = 2.0f div (top - bottom)
  m.m[2][2] = -(2.0f div (farZ - nearZ))
  m.m[0][3] = -((right + left) div (right - left))
  m.m[1][3] = -((top + bottom) div (top - bottom))
  m.m[2][3] = -((farZ + nearZ) div (farZ - nearZ))
  return m

##  ======== Camera (LookAt) ========

proc mat4f_look_at*(eye: Vec3f; center: Vec3f; up: Vec3f): Mat4f {.inline.} =
  var f: Vec3f = vec3f_normalize(vec3f_sub(center, eye))
  var s: Vec3f = vec3f_normalize(vec3f_cross(f, up))
  var u: Vec3f = vec3f_cross(s, f)
  var m: Mat4f = mat4f_identity()
  m.m[0][0] = s.x
  m.m[0][1] = s.y
  m.m[0][2] = s.z
  m.m[1][0] = u.x
  m.m[1][1] = u.y
  m.m[1][2] = u.z
  m.m[2][0] = -f.x
  m.m[2][1] = -f.y
  m.m[2][2] = -f.z
  m.m[0][3] = -vec3f_dot(s, eye)
  m.m[1][3] = -vec3f_dot(u, eye)
  m.m[2][3] = vec3f_dot(f, eye)
  return m

##  ======== Utility Functions ========

proc vec3f_lerp*(a: Vec3f; b: Vec3f; t: cfloat): Vec3f {.inline.} =
  return vec3f_add(vec3f_scale(a, 1 - t), vec3f_scale(b, t))

proc quat_slerp*(a: Quatf; b: Quatf; t: cfloat): Quatf {.inline.} =
  var dot: cfloat = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w
  if dot < 0.0f:
    b.x = -b.x
    b.y = -b.y
    b.z = -b.z
    b.w = -b.w
    dot = -dot
  if dot > 0.9995f:
    return quat_from_axis_angle(vec3f_normalize((constructVec3f),
        (b.x - a.x, b.y - a.y, b.z - a.z)), t)
  var
    theta_0: cfloat = acosf(dot)
    theta: cfloat = theta_0 * t
  var
    sin_theta: cfloat = sinf(theta)
    sin_theta_0: cfloat = sinf(theta_0)
  var s0: cfloat = cosf(theta) - dot * sin_theta div sin_theta_0
  var s1: cfloat = sin_theta div sin_theta_0
  ## !!!Ignored construct:  return ( Quatf ) { a . x * s0 + b . x * s1 , a . y * s0 + b . y * s1 , a . z * s0 + b . z * s1 , a . w * s0 + b . w * s1 } ;
  ## Error: token expected: ; but got: {!!!
  