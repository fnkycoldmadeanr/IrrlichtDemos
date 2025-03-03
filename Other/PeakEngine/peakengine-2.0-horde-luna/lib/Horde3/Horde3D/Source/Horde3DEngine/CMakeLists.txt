
# CMAKE_BINARY_DIR is used for inclusion of automatically generated files
include_directories(. ../Shared ../../../ ${CMAKE_BINARY_DIR})


set(HORDE3D_SOURCES 
	egAnimatables.cpp
	egAnimation.cpp
	egCamera.cpp
	egCom.cpp
	egExtensions.cpp
	egGeometry.cpp
	egLight.cpp
	egMain.cpp
	egMaterial.cpp
	egModel.cpp
	egModules.cpp
	egParticle.cpp
	egPipeline.cpp
	egPrimitives.cpp
	egRendererBase.cpp
	egRenderer.cpp
	egResource.cpp
	egScene.cpp
	egSceneGraphRes.cpp
	egShader.cpp
	egTextures.cpp
	utImage.cpp
	utOpenGL.cpp
	egAnimatables.h
	egAnimation.h
	egCamera.h
	egCom.h
	egExtensions.h
	egGeometry.h
	egLight.h
	egMaterial.h
	egModel.h
	egModules.h
	egParticle.h
	egPipeline.h
	egPrerequisites.h
	egPrimitives.h
	egRenderer.h
	egRendererBase.h
	egResource.h
	egScene.h
	egSceneGraphRes.h
	egShader.h
	egTextures.h
	utImage.h
	utOpenGL.h
	../Shared/utXMLParser.cpp

	${HORDE3D_EXTENSION_SOURCES}
	)

add_definitions(-DCMAKE )

add_library(Horde3D STATIC
	${HORDE3D_SOURCES}
	)


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(Horde3D opengl32)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(Horde3D GL)
	install(TARGETS Horde3D
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
	FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)
	target_link_libraries(Horde3D ${OPENGL_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
