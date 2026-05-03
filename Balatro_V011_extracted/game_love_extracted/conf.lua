_RELEASE_MODE = false
_JIT = true --true = normal / jit off
_DEMO = false
_QUE_SOUND = true --true = normal
_QUE_SPLASH = false --true = normal
_QUE_SHADER = true --true = normal
_QUE_MIPMAPS = false --true = normal
_QUE_SKIPTUTORIAL = false
_QUE_BOOTTIMER = true --true = normal


_QUE_REDABLECANVAS = false --true = normal should be left false
_DEBUG_TOOLS = false
function love.conf(t)
	t.console = not _RELEASE_MODE
	t.window.width = 960
    t.window.height = 544
	t.version = "11.4"
end
