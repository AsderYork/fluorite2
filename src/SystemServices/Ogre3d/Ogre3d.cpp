#include <SystemServices/Ogre3d/Ogre3d.hpp>
#include <SystemServices/Ogre3d/Ogre3dCameraControl/Ogre3dCameraControl.hpp>

#include <Terrain/Terrain.hpp>
#include <iostream>

namespace fluorite
{

    std::vector<Ogre::SceneNode*> thingsToRotate;
    Ogre3dCameraControll* camcontrol;


    /**
     * Simple class to for creation of some test cubes
     */
    class DebugMeshesGenerator {
    private:
        static bool isInited;

        static void createColourCube() {
                /// Create the mesh via the MeshManager
                Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual("ColourCube", "General");

                /// Create one submesh
                Ogre::SubMesh* sub = msh->createSubMesh();

                const float sqrt13 = 0.577350269f; /* sqrt(1/3) */

                /// Define the vertices (8 vertices, each have 3 floats for position and 3 for normal)
                const size_t nVertices = 8;
                const size_t vbufCount = 3*2*nVertices;
                float vertices[vbufCount] = {
                        -1.0,1.0,-1.0,        //0 position
                        -sqrt13,sqrt13,-sqrt13,     //0 normal
                        1.0,1.0,-1.0,         //1 position
                        sqrt13,sqrt13,-sqrt13,      //1 normal
                        1.0,-1.0,-1.0,        //2 position
                        sqrt13,-sqrt13,-sqrt13,     //2 normal
                        -1.0,-1.0,-1.0,       //3 position
                        -sqrt13,-sqrt13,-sqrt13,    //3 normal
                        -1.0,1.0,1.0,         //4 position
                        -sqrt13,sqrt13,sqrt13,      //4 normal
                        1.0,1.0,1.0,          //5 position
                        sqrt13,sqrt13,sqrt13,       //5 normal
                        1.0,-1.0,1.0,         //6 position
                        sqrt13,-sqrt13,sqrt13,      //6 normal
                        -1.0,-1.0,1.0,        //7 position
                        -sqrt13,-sqrt13,sqrt13,     //7 normal
                };

                Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
                Ogre::RGBA colours[nVertices];
                Ogre::RGBA *pColour = colours;
                // Use render system to convert colour value since colour packing varies
                rs->convertColourValue(Ogre::ColourValue(1.0,0.0,0.0), pColour++); //0 colour
                rs->convertColourValue(Ogre::ColourValue(1.0,1.0,0.0), pColour++); //1 colour
                rs->convertColourValue(Ogre::ColourValue(0.0,1.0,0.0), pColour++); //2 colour
                rs->convertColourValue(Ogre::ColourValue(0.0,0.0,0.0), pColour++); //3 colour
                rs->convertColourValue(Ogre::ColourValue(1.0,0.0,1.0), pColour++); //4 colour
                rs->convertColourValue(Ogre::ColourValue(1.0,1.0,1.0), pColour++); //5 colour
                rs->convertColourValue(Ogre::ColourValue(0.0,1.0,1.0), pColour++); //6 colour
                rs->convertColourValue(Ogre::ColourValue(0.0,0.0,1.0), pColour++); //7 colour

                /// Define 12 triangles (two triangles per cube face)
                /// The values in this table refer to vertices in the above table
                const size_t ibufCount = 36;
                unsigned short faces[ibufCount] = {
                        0,2,3,
                        0,1,2,
                        1,6,2,
                        1,5,6,
                        4,6,5,
                        4,7,6,
                        0,7,4,
                        0,3,7,
                        0,5,1,
                        0,4,5,
                        2,7,3,
                        2,6,7
                };

                /// Create vertex data structure for 8 vertices shared between submeshes
                msh->sharedVertexData = new Ogre::VertexData();
                msh->sharedVertexData->vertexCount = nVertices;

                /// Create declaration (memory format) of vertex data
                Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
                size_t offset = 0;
                // 1st buffer
                decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
                decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
                offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
                /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
                /// and bytes per vertex (offset)
                Ogre::HardwareVertexBufferSharedPtr vbuf = 
                    Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                    offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
                /// Upload the vertex data to the card
                vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

                /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
                Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
                bind->setBinding(0, vbuf);

                // 2nd buffer
                offset = 0;
                decl->addElement(1, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
                offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
                /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
                /// and bytes per vertex (offset)
                vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                    offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
                /// Upload the vertex data to the card
                vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);

                /// Set vertex buffer binding so buffer 1 is bound to our colour buffer
                bind->setBinding(1, vbuf);

                /// Allocate index buffer of the requested number of vertices (ibufCount) 
                Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
                    createIndexBuffer(
                    Ogre::HardwareIndexBuffer::IT_16BIT, 
                    ibufCount, 
                    Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

                /// Upload the index data to the card
                ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

                /// Set parameters of the submesh
                sub->useSharedVertices = true;
                sub->indexData->indexBuffer = ibuf;
                sub->indexData->indexCount = ibufCount;
                sub->indexData->indexStart = 0;

                /// Set bounding information (for culling)
                msh->_setBounds(Ogre::AxisAlignedBox(-1,-1,-1,1,1,1));
                msh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3));

                /// Notify -Mesh object that it has been loaded
                msh->load();
            }

        static void createMeshFromVertices(std::string name, std::vector<Ogre::Vector3> vertices, std::vector<int> indices, Ogre::Vector3 size = {1,1,1}) {
            /// Create the mesh via the MeshManager
            Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(name, "General");

            /// Create one submesh
            Ogre::SubMesh* sub = msh->createSubMesh();


            Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

            /// Create vertex data structure for 8 vertices shared between submeshes
            msh->sharedVertexData = new Ogre::VertexData();
            msh->sharedVertexData->vertexCount = vertices.size();

            /// Create declaration (memory format) of vertex data
            Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
            size_t offset = 0;
            // 1st buffer
            decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
            offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
            //decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
            //offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
            /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
            /// and bytes per vertex (offset)
            Ogre::HardwareVertexBufferSharedPtr vbuf = 
                Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            /// Upload the vertex data to the card
            vbuf->writeData(0, vbuf->getSizeInBytes(), vertices.data(), true);

            /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
            Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
            bind->setBinding(0, vbuf);

            /// Allocate index buffer of the requested number of vertices (ibufCount) 
            Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
                createIndexBuffer(
                Ogre::HardwareIndexBuffer::IT_32BIT, 
                indices.size(), 
                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            /// Upload the index data to the card
            ibuf->writeData(0, ibuf->getSizeInBytes(), indices.data(), true);

            /// Set parameters of the submesh
            sub->useSharedVertices = true;
            sub->indexData->indexBuffer = ibuf;
            sub->indexData->indexCount = indices.size();
            sub->indexData->indexStart = 0;

            /// Set bounding information (for culling)
            msh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(0), size));
            msh->_setBoundingSphereRadius(size.length());

            /// Notify -Mesh object that it has been loaded
            msh->load();
        }

        static void createTestMaterial() {
            Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Test/ColourTest", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            material->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
            material->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_AMBIENT);
            
        }

        Ogre::SceneNode* putMesh(Ogre::String name, Ogre::Vector3 pos, std::vector<Ogre::Vector3> vertices, std::vector<int> indices, Ogre::Vector3 size = {1,1,1}, Ogre::ColourValue color = Ogre::ColourValue::ZERO) {

            if(pos.x == 16 && pos.y == 0 && pos.z == 0) {
                        std::cout << name;
            }

            createMeshFromVertices(name, vertices, indices, size);

            auto mainSceneManager = Ogre::Root::getSingletonPtr()->getSceneManagers().begin()->second;

            auto thisEntity = mainSceneManager->createEntity(name, name);

            if(color == Ogre::ColourValue::ZERO) {
                thisEntity->setMaterialName("Test/ColourTest");
            } else {
                Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Test/" + thisEntity->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                material->getTechnique(0)->getPass(0)->setAmbient(color);
                thisEntity->setMaterial(material);
            }
            auto thisSceneNode = mainSceneManager->getRootSceneNode()->createChildSceneNode();
            thisSceneNode->setPosition(pos);
            thisSceneNode->attachObject(thisEntity);

            return thisSceneNode;
        }

    public:
        static Ogre::SceneNode* putTestCube(Ogre::String name, Ogre::Vector3 pos, Ogre::Vector3 size = {1,1,1}, Ogre::ColourValue color = Ogre::ColourValue::ZERO) {

            if(!isInited) {
                createColourCube();
                createTestMaterial();
                isInited = true;
            }

            auto mainSceneManager = Ogre::Root::getSingletonPtr()->getSceneManagers().begin()->second;

            auto thisEntity = name.size() > 0 ? mainSceneManager->createEntity(name, "ColourCube") : mainSceneManager->createEntity("ColourCube");

            if(color == Ogre::ColourValue::ZERO) {
                thisEntity->setMaterialName("Test/ColourTest");
            } else {
                Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Test/" + thisEntity->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                material->getTechnique(0)->getPass(0)->setAmbient(color);
                material->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
                thisEntity->setMaterial(material);
            }
            auto thisSceneNode = mainSceneManager->getRootSceneNode()->createChildSceneNode();
            thisSceneNode->setPosition(pos);
            thisSceneNode->attachObject(thisEntity);
            thisSceneNode->scale(size);

            return thisSceneNode;
        }    
    };
    bool DebugMeshesGenerator::isInited = false;

    

    bool Ogre3d::initOgre(SDL2Controller* sdl) {

        sdlController = sdl;
        auto root = new Ogre::Root("", "");
        auto glRender = new Ogre::GLRenderSystem();
        root->addRenderSystem(glRender);	
        root->setRenderSystem(glRender);	

        Ogre::NameValuePairList params; 
        params["externalWindowHandle"] = Ogre::StringConverter::toString(sdl->getWindowIdentifier()); 
        root->initialise(false); 
        auto mWindow = root->createRenderWindow("View", 800, 600, false, &params); 

        Ogre::SceneManager *sceneMgr = root->createSceneManager();
        

        camcontrol = new Ogre3dCameraControll(sceneMgr, sdlController);       
        

        Ogre::Viewport *vp = mWindow->addViewport(camcontrol->getCamera());


        sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        thingsToRotate.push_back(DebugMeshesGenerator::putTestCube("cc", Ogre::Vector3(-8, 0, 2)));
        thingsToRotate.push_back(DebugMeshesGenerator::putTestCube("cc2", Ogre::Vector3(-8, 0, -2)));
       
        return true;
    }

    void Ogre3d::ogreFrame(float delta) {
        for(auto obj : thingsToRotate) {
            obj->yaw(Ogre::Radian(delta * 0.5f));
        }

        camcontrol->frame(delta);

        Ogre::Root::getSingletonPtr()->renderOneFrame(delta);
    }    

    void Ogre3d::ogreShutdown() {
    }


}
