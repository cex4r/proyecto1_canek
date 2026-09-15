project('multichat_server', 'c',
  default_options: ['c_std=c11', 'warning_level=3'])

thread_dep = dependency('threads')
cmocka_dep = dependency('cmocka')

# Dejando que el main executable y los tests encuentren los headers
inc = include_directories('src')

model_sources = files(
  'src/modelo/usuario.c',
  'src/modelo/sala.c',
  'src/modelo/servidor.c',
)

# --- Main server executable ---
servidor_exe = executable('servidor',
  'src/main.c',
  model_sources,
  include_directories: inc,
  dependencies: thread_dep,
)

# --- Unit tests ---
test_usuario_exe = executable('test_usuario',
  'tests/test_usuario.c',
  model_sources,
  include_directories: inc,
  dependencies: [thread_dep, cmocka_dep],
)

test('usuario', test_usuario_exe)