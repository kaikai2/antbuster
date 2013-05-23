-- target 是预定义变量,表示编译环境的名字
project.path = "../build/" .. target
project.name = "antbuster"
project.bindir = "../bin"
project.libdir = "../lib"

-----------------------------
-- Antbuster package
-----------------------------
package = newpackage()

package.path = "../build/" .. target
package.kind = "winexe"
package.name = "antbuster"
package.language = "c++"
package.bindir = "../../bin"

package.config["Debug"].objdir = "./Debug/demo"
package.config["Debug"].target = "antbuster_d"
package.config["Release"].objdir = "./Release/demo"
package.config["Release"].target = "antbuster"

package.linkoptions ={ "/NODEFAULTLIB:libc" }
package.buildflags = {"no-main", "extra-warnings", "static-runtime", "no-exceptions", "no-rtti" }
package.config["Debug"].links = { "hge", "hgehelp", "curvedani_d", "hgeport_d" }
package.config["Release"].links = { "hge", "hgehelp", "curvedani", "hgeport" }
package.includepaths = { "../../include", "../../include/ca", "../../include/hge" }

package.libpaths = { "../../lib", "../../lib/" .. target } 

package.files = {
  matchrecursive("../../include/*.h", "../../src/*.cpp")
}
