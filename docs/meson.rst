Meson
=====

We build our userspace dependencies with meson, to make each lib self contained and aware of it's dependencies and it's settings.
Building all via meson has this significant advantage that we can manage the build of the dependency while building an app, so we
can do things like change dependency configuratoion.

So current architecture for building apps in userspace looks like this:
   
buildroot -> meson -> app -> lib1 -> lib1.a -> app
			     lib2    lib2.a
		             ...     ...

Buidlroot manages build of the app. App manages build of it's depenencies. If you need to add another app add it to buildroot as a package,
if you need another lib add it to the app itself as meson dependency.

