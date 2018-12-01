/*
  (C) 2018 David Lettier
  lettier.com
*/

#include <iostream>

#include "pandaFramework.h"
#include "pandaSystem.h"

#include "asyncTaskManager.h"
#include "genericAsyncTask.h"

#include "ambientLight.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "spotlight.h"

#include "lightRampAttrib.h"

#include "auto_bind.h"
#include "animControlCollection.h"

#include "randomizer.h"

#include "shader.h"

#include "mouseWatcher.h"
#include "buttonRegistry.h"

#include "collisionTraverser.h"
#include "collisionHandlerPusher.h"
#include "collisionNode.h"
#include "collisionSphere.h"
#include "collisionBox.h"

#include "audioManager.h"
#include "audioSound.h"

#include "character.h"

#include "fontPool.h"
#include "dynamicTextFont.h"

#include "purescript.h"

PT(AsyncTaskManager) taskMgr = AsyncTaskManager::get_global_ptr();
PT(ClockObject) globalClock = ClockObject::get_global_clock();

CollisionTraverser collisionTraverser;
PT(CollisionHandlerPusher) collisionHandlerPusher = new CollisionHandlerPusher();

PT(AudioManager) audioManager = AudioManager::create_AudioManager();

AsyncTask::DoneStatus GenericAsyncTaskFunc(
  GenericAsyncTask* task,
  void* data
) {
  // TODO: Delete data if the task is done.
  auto box = static_cast<purescript::boxed*>(data);
  auto& function = *static_cast<std::function<purescript::boxed(void)>*>(box->get());
  function();
  return AsyncTask::DS_cont;
}

// Needed for Panda3D 1.9.4 and below.
// PandaFramework::open_framework() does not exist.
std::string __program_name = "lambda-lantern";
char* __args[] = {const_cast<char*>(__program_name.c_str()), nullptr};
int argc = sizeof(__args) / sizeof(__args[0]) - 1;
char** argv = __args;

FOREIGN_BEGIN( Panda3d )

exports["withFramework"] = [](const boxed& f_) -> boxed {
  return [=]() -> boxed {
    PandaFramework* framework = new PandaFramework();
    framework->open_framework(argc, argv);
    f_(box<PandaFramework*>(framework))();
    framework->main_loop();
    audioManager->shutdown();
    framework->close_framework();
    delete framework;
    return boxed();
  };
};

exports["setWindowTitle"] = [](const boxed& framework_) -> boxed {
  return [&](const boxed& title_) -> boxed {
    return [&]() -> boxed {
      const auto& framework = unbox<PandaFramework*>(framework_);
      const auto title = unbox<string>(title_);
      framework->set_window_title(title);
      return boxed();
    };
  };
};

exports["withWindow"] = [](const boxed& framework_) -> boxed {
  return [&](const boxed& f) -> boxed {
    return [&]() -> boxed {
      const auto& framework = unbox<PandaFramework*>(framework_);
      WindowFramework* window = framework->open_window();
      f(box<WindowFramework*>(window))();
      return boxed();
    };
  };
};

exports["getCamera"] = [](const boxed& window_) -> boxed {
  return [&]() -> boxed {
    const auto& window = unbox<WindowFramework*>(window_);
    auto nodePath = window->get_camera_group();
    return box<NodePath>(nodePath);
  };
};

exports["getDisplayRegion"] = [](const boxed& window_) -> boxed {
  return [&]() -> boxed {
    const auto& window = unbox<WindowFramework*>(window_);
    auto displayRegionPtr = window->get_display_region_3d();
    return box<DisplayRegion*>(displayRegionPtr);
  };
};

exports["getModels"] = [](const boxed& framework_) -> boxed {
  return [&]() -> boxed {
    const auto& framework = unbox<PandaFramework*>(framework_);
    auto nodePath = framework->get_models();
    return box<NodePath>(nodePath);
  };
};

exports["getRender"] = [](const boxed& window_) -> boxed {
  return [&]() -> boxed {
    const auto& window = unbox<WindowFramework*>(window_);
    auto nodePath = window->get_render();
    return box<NodePath>(nodePath);
  };
};

exports["getLens"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto& nodePath = unbox<NodePath>(nodePath_);
    auto node = nodePath.node();
    auto numChildren = node->get_num_children();
    Lens* lensPtr;
    // TODO: test for camera, light, and light with frustum.
    if (numChildren > 0 && !node->get_child(0)->is_geom_node()) {
      lensPtr = DCAST(LensNode, node->get_child(0))->get_lens();
    } else {
      lensPtr = DCAST(LensNode, node)->get_lens();
    }
    return box<Lens*>(lensPtr);
  };
};

exports["getSound"] = [](const boxed& path_) -> boxed {
  return [&]() -> boxed {
    const auto& path = unbox<string>(path_);
    PT(AudioSound) sound = audioManager->get_sound(path);
    return box<PT(AudioSound)>(sound);
  };
};

exports["playSound"] = [](const boxed& sound_) -> boxed {
  return [&]() -> boxed {
    const auto& sound = unbox<PT(AudioSound)>(sound_);
    sound->play();
    return boxed();
  };
};

exports["stopSound"] = [](const boxed& sound_) -> boxed {
  return [&]() -> boxed {
    const auto& sound = unbox<PT(AudioSound)>(sound_);
    sound->stop();
    return boxed();
  };
};

exports["setSoundVolume"] = [](const boxed& sound_) -> boxed {
  return [&](const boxed& volume_) -> boxed {
    return [&]() -> boxed {
      const auto& sound = unbox<PT(AudioSound)>(sound_);
      const auto volume = unbox<double>(volume_);
      sound->set_volume(volume);
      return boxed();
    };
  };
};

exports["setSoundLoop"] = [](const boxed& sound_) -> boxed {
  return [&](const boxed& isLoop_) -> boxed {
    return [&]() -> boxed {
      const auto& sound = unbox<PT(AudioSound)>(sound_);
      const auto isLoop = unbox<bool>(isLoop_);
      sound->set_loop(isLoop);
      return boxed();
    };
  };
};

exports["loopAnimations"] = [](const boxed& window_) -> boxed {
  return [&](const boxed& i_) -> boxed {
    return [&]() -> boxed {
      const auto& window = unbox<WindowFramework*>(window_);
      const auto i = unbox<int>(i_);
      window->loop_animations(i);
      return boxed();
    };
  };
};

exports["loopAnimation"] = [](const boxed& animations_) -> boxed {
  return [&](const boxed& animationName_) -> boxed {
    return [&](const boxed& restart_) -> boxed {
      return [&]() -> boxed {
        const auto& animations = unbox<AnimControlCollection*>(animations_);
        const auto animationName = unbox<string>(animationName_);
        const auto restart = unbox<bool>(restart_);
        animations->loop(animationName, restart);
        return boxed();
      };
    };
  };
};

exports["stopAnimation"] = [](const boxed& animations_) -> boxed {
  return [&](const boxed& animationName_) -> boxed {
    return [&]() -> boxed {
      const auto& animations = unbox<AnimControlCollection*>(animations_);
      const auto animationName = unbox<string>(animationName_);
      animations->play(animationName);
      return boxed();
    };
  };
};

exports["playAnimation"] = [](const boxed& animations_) -> boxed {
  return [&](const boxed& animationName_) -> boxed {
    return [&]() -> boxed {
      const auto& animations = unbox<AnimControlCollection*>(animations_);
      const auto animationName = unbox<string>(animationName_);
      animations->play(animationName);
      return boxed();
    };
  };
};

exports["loadModel"] = [](const boxed& window_) -> boxed {
  return [&](const boxed& nodePath_) -> boxed {
    return [&](const boxed& s_) -> boxed {
      return [&]() -> boxed {
        const auto& window = unbox<WindowFramework*>(window_);
        auto nodePath = unbox<NodePath>(nodePath_);
        const auto s = unbox<string>(s_);
        auto newNodePath = window->load_model(nodePath, s);
        return box<NodePath>(newNodePath);
      };
    };
  };
};

exports["loadFont"] = [](const boxed& path_) -> boxed {
  return [&]() -> boxed {
    const auto path = unbox<string>(path_);
    PT(TextFont) font = FontPool::load_font(path);;
    PT(DynamicTextFont) dynamicFont = DCAST(DynamicTextFont, font);
    dynamicFont->set_pixels_per_unit(100);
    dynamicFont->set_page_size(1024, 1024);
    return box<PT(TextFont)>(font);
  };
};

exports["loadAnimations"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    AnimControlCollection* animations = new AnimControlCollection();
    auto_bind(
      nodePath.node(),
      *animations,
      // TODO: Parameter
      PartGroup::HMF_ok_wrong_root_name
    );
    return box<AnimControlCollection*>(animations);
  };
};

// TODO: CG versus GLSL parameter
exports["loadShader"] = [](const boxed& path_) -> boxed {
  return [&]() -> boxed {
    const auto path = unbox<string>(path_);
    PT(Shader) shader = Shader::load(path, Shader::SL_Cg);
    return box<PT(Shader)>(shader);
  };
};

exports["createPointLight"] = [](const boxed& name_) -> boxed {
  return [&](const boxed& isShadowCaster_) -> boxed {
    return [&]() -> boxed {
      const auto name = unbox<string>(name_);
      const auto isShadowCaster = unbox<bool>(isShadowCaster_);
      PointLight* l = new PointLight(name);
      if (isShadowCaster) {
        l->set_shadow_caster(isShadowCaster, 1024, 1024);
      }
      PandaNode* pandaNode = l->as_node();
      return box<PandaNode*>(pandaNode);
    };
  };
};

exports["createDirectionalLight"] = [](const boxed& name_) -> boxed {
  return [&](const boxed& isShadowCaster_) -> boxed {
    return [&]() -> boxed {
      const auto name = unbox<string>(name_);
      const auto isShadowCaster = unbox<bool>(isShadowCaster_);
      DirectionalLight* l = new DirectionalLight(name);
      if (isShadowCaster) {
        l->set_shadow_caster(isShadowCaster, 1024, 1024);
      }
      PandaNode* pandaNode = l->as_node();
      return box<PandaNode*>(pandaNode);
    };
  };
};

exports["createSpotlight"] = [](const boxed& name_) -> boxed {
  return [&](const boxed& isShadowCaster_) -> boxed {
    return [&]() -> boxed {
      const auto name = unbox<string>(name_);
      const auto isShadowCaster = unbox<bool>(isShadowCaster_);
      Spotlight* l = new Spotlight(name);
      if (isShadowCaster) {
        l->set_shadow_caster(isShadowCaster, 1024, 1024);
      }
      PandaNode* pandaNode = l->as_node();
      return box<PandaNode*>(pandaNode);
    };
  };
};

exports["createAmbientLight"] = [](const boxed& s_) -> boxed {
  return [&](const boxed& r_) -> boxed {
    return [&](const boxed& g_) -> boxed {
      return [&](const boxed& b_) -> boxed {
        return [&]() -> boxed {
          const auto s = unbox<string>(s_);
          const auto r = unbox<double>(r_);
          const auto g = unbox<double>(g_);
          const auto b = unbox<double>(b_);
          AmbientLight* l = new AmbientLight(s);
          l->set_color(LVecBase4f(r, g, b, 1));
          PandaNode* pandaNode = l->as_node();
          return box<PandaNode*>(pandaNode);
        };
      };
    };
  };
};

exports["reparentTo"] = [](const boxed& parentNodePath_) -> boxed {
  return [&](const boxed& childNodePath_) -> boxed {
    return [&]() -> boxed {
      const auto& parentNodePath = unbox<NodePath>(parentNodePath_);
      auto childNodePath = unbox<NodePath>(childNodePath_);
      childNodePath.reparent_to(parentNodePath);
      return boxed();
    };
  };
};

exports["attachNewNode"] = [](const boxed& parentNodePath_) -> boxed {
  return [&](const boxed& pandaNode_) -> boxed {
    return [&]() -> boxed {
      auto parentNodePath = unbox<NodePath>(parentNodePath_);
      const auto& pandaNode = unbox<PandaNode*>(pandaNode_);
      auto newNodePath = parentNodePath.attach_new_node(pandaNode);
      return box<NodePath>(newNodePath);
    };
  };
};

exports["setScale"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& x_) mutable -> boxed {
    return [&](const boxed& y_) mutable -> boxed {
      return [&](const boxed& z_) mutable -> boxed {
        return [&]() mutable -> boxed {
          auto nodePath = unbox<NodePath>(nodePath_);
          const auto x = unbox<double>(x_);
          const auto y = unbox<double>(y_);
          const auto z = unbox<double>(z_);
          nodePath.set_scale(x, y, z);
          return boxed();
        };
      };
    };
  };
};

exports["setPosition"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& x_) -> boxed {
    return [&](const boxed& y_) -> boxed {
      return [&](const boxed& z_) -> boxed {
        return [&]() -> boxed {
          auto nodePath = unbox<NodePath>(nodePath_);
          const auto x = unbox<double>(x_);
          const auto y = unbox<double>(y_);
          const auto z = unbox<double>(z_);
          nodePath.set_pos(x, y, z);
          return boxed();
        };
      };
    };
  };
};

exports["setHpr"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& h_) -> boxed {
    return [&](const boxed& p_) -> boxed {
      return [&](const boxed& r_) -> boxed {
        return [&]() -> boxed {
          auto nodePath = unbox<NodePath>(nodePath_);
          const auto h = unbox<double>(h_);
          const auto p = unbox<double>(p_);
          const auto r = unbox<double>(r_);
          nodePath.set_hpr(h, p, r);
          return boxed();
        };
      };
    };
  };
};

exports["setLight"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& light_) -> boxed {
    return [&]() -> boxed {
      const auto& light = unbox<NodePath>(light_);
      auto nodePath = unbox<NodePath>(nodePath_);
      nodePath.set_light(light);
      return boxed();
    };
  };
};

exports["setFov"] = [](const boxed& lens_) -> boxed {
  return [&](const boxed& fov_) -> boxed {
    return [&]() -> boxed {
      const auto& lens = unbox<Lens*>(lens_);
      const auto fov = unbox<double>(fov_);
      lens->set_fov(fov);
      return boxed();
    };
  };
};

exports["setShaderAuto"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    nodePath.set_shader_auto();
    return boxed();
  };
};

exports["setShader"] = [](const boxed& shader_) -> boxed {
  return [&](const boxed& nodePath_) -> boxed {
    return [&]() -> boxed {
      const auto& shader = unbox<PT(Shader)>(shader_);
      auto nodePath = unbox<NodePath>(nodePath_);
      nodePath.set_shader(shader);
      return boxed();
    };
  };
};

exports["setShaderInputNodePath"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& variable_) -> boxed {
    return [&](const boxed& inputNodePath_) -> boxed {
      return [&]() -> boxed {
        auto nodePath = unbox<NodePath>(nodePath_);
        const auto variable = unbox<string>(variable_);
        auto inputNodePath = unbox<NodePath>(inputNodePath_);
        nodePath.set_shader_input(variable, inputNodePath);
        return boxed();
      };
    };
  };
};

exports["setColor"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& r_) -> boxed {
    return [&](const boxed& g_) -> boxed {
      return [&](const boxed& b_) -> boxed {
        return [&]() -> boxed {
          auto nodePath = unbox<NodePath>(nodePath_);
          const auto r = unbox<double>(r_);
          const auto g = unbox<double>(g_);
          const auto b = unbox<double>(b_);
          nodePath.node()->as_light()->set_color(LColor(r, g, b, 1.0));
          return boxed();
        };
      };
    };
  };
};

// TODO: Only certain lights can do this. Create an interface.
exports["setAttenutation"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& c_) -> boxed {
    return [&](const boxed& l_) -> boxed {
      return [&](const boxed& q_) -> boxed {
        return [&]() -> boxed {
          auto nodePath = unbox<NodePath>(nodePath_);
          const auto c = unbox<double>(c_);
          const auto l = unbox<double>(l_);
          const auto q = unbox<double>(q_);
          DCAST(PointLight, nodePath.node())->set_attenuation(LVecBase3(c, l, q));
          return boxed();
        };
      };
    };
  };
};

exports["setClearColor"] = [](const boxed& displayRegion_) -> boxed {
  return [&](const boxed& r_) -> boxed {
    return [&](const boxed& g_) -> boxed {
      return [&](const boxed& b_) -> boxed {
        return [&]() -> boxed {
          const auto& displayRegion = unbox<DisplayRegion*>(displayRegion_);
          const auto r = unbox<double>(r_);
          const auto g = unbox<double>(g_);
          const auto b = unbox<double>(b_);
          displayRegion->set_clear_color(LColor(r, g, b, 1.0));
          return boxed();
        };
      };
    };
  };
};

exports["setLightRampAttrib"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& t0_) -> boxed {
    return [&](const boxed& l0_) -> boxed {
      return [&](const boxed& t1_) -> boxed {
        return [&](const boxed& l1_) -> boxed {
          return [&]() -> boxed {
            auto nodePath = unbox<NodePath>(nodePath_);
            const auto t0 = unbox<double>(t0_);
            const auto l0 = unbox<double>(l0_);
            const auto t1 = unbox<double>(t1_);
            const auto l1 = unbox<double>(l1_);
            nodePath.set_attrib(LightRampAttrib::make_double_threshold(t0, l0, t1, l1));
            return boxed();
          };
        };
      };
    };
  };
};

exports["getPosition"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    auto position = nodePath.get_pos();
    auto dictionary = string_literal_dict_t<boxed>();
    dictionary.push_back(std::make_pair("x", boxed(position.get_x())));
    dictionary.push_back(std::make_pair("y", boxed(position.get_y())));
    dictionary.push_back(std::make_pair("z", boxed(position.get_z())));
    return boxed(dictionary);
  };
};

exports["getHpr"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    auto hpr = nodePath.get_hpr();
    auto dictionary = string_literal_dict_t<boxed>();
    dictionary.push_back(std::make_pair("h", boxed(hpr.get_x())));
    dictionary.push_back(std::make_pair("p", boxed(hpr.get_y())));
    dictionary.push_back(std::make_pair("r", boxed(hpr.get_z())));
    return boxed(dictionary);
  };
};

exports["lookAt"] = [](const boxed& at_) -> boxed {
  return [&](const boxed& look_) -> boxed {
    return [&]() -> boxed {
      auto at = unbox<NodePath>(at_);
      auto look = unbox<NodePath>(look_);
      look.look_at(at);
      return boxed();
    };
  };
};

exports["addGenericAsyncTask"] = [](const boxed& s_) -> boxed {
  return [&](const boxed& f) -> boxed {
    return [&]() -> boxed {
      const auto s = unbox<string>(s_);
      void* fPtr = new boxed(f);
      GenericAsyncTask* task = new GenericAsyncTask(
        s,
        &GenericAsyncTaskFunc,
        fPtr
      );
      taskMgr->add(task);
      return boxed();
    };
  };
};

exports["addCollisionSphere"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& name_) -> boxed {
    return [&](const boxed& x_) -> boxed {
      return [&](const boxed& y_) -> boxed {
        return [&](const boxed& z_) -> boxed {
          return [&](const boxed& r_) -> boxed {
            return [&](const boxed& show_) -> boxed {
              return [&]() -> boxed {
                auto nodePath = unbox<NodePath>(nodePath_);
                const auto name = unbox<string>(name_);
                const auto x = unbox<double>(x_);
                const auto y = unbox<double>(y_);
                const auto z = unbox<double>(z_);
                const auto r = unbox<double>(r_);
                const auto show = unbox<bool>(show_);
                PT(CollisionSphere) colSphere = new CollisionSphere(x, y, z, r);
                PT(CollisionNode) colNode = new CollisionNode(name);
                colNode->add_solid(colSphere);
                auto newNodePath = nodePath.attach_new_node(colNode);
                if (show) {
                  newNodePath.show();
                }
                return box<NodePath>(newNodePath);
              };
            };
          };
        };
      };
    };
  };
};

exports["addCollisionBox"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& name_) -> boxed {
    return [&](const boxed& cx_) -> boxed {
      return [&](const boxed& cy_) -> boxed {
        return [&](const boxed& cz_) -> boxed {
          return [&](const boxed& dx_) -> boxed {
            return [&](const boxed& dy_) -> boxed {
              return [&](const boxed& dz_) -> boxed {
                return [&](const boxed& show_) -> boxed {
                  return [&]() -> boxed {
                    auto nodePath = unbox<NodePath>(nodePath_);
                    const auto name = unbox<string>(name_);
                    const auto cx = unbox<double>(cx_);
                    const auto cy = unbox<double>(cy_);
                    const auto cz = unbox<double>(cz_);
                    const auto dx = unbox<double>(dx_);
                    const auto dy = unbox<double>(dy_);
                    const auto dz = unbox<double>(dz_);
                    const auto show = unbox<bool>(show_);
                    PT(CollisionBox) colBox = new CollisionBox(LPoint3(cx, cy, cz), dx, dy, dz);
                    PT(CollisionNode) colNode = new CollisionNode(name);
                    colNode->add_solid(colBox);
                    auto newNodePath = nodePath.attach_new_node(colNode);
                    if (show) {
                      newNodePath.show();
                    }
                    return box<NodePath>(newNodePath);
                  };
                };
              };
            };
          };
        };
      };
    };
  };
};

exports["addColliderToTraverserAndPusher"] = [](const boxed& target_) -> boxed {
  return [&](const boxed& collider_) -> boxed {
    return [&]() -> boxed {
      auto target = unbox<NodePath>(target_);
      auto collider = unbox<NodePath>(collider_);
      collisionHandlerPusher->add_collider(collider, target);
      collisionTraverser.add_collider(collider, collisionHandlerPusher);
      return boxed();
    };
  };
};

exports["traverseCollisionTraverser"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    collisionTraverser.traverse(nodePath);
    return boxed();
  };
};

exports["findButton"] = [](const boxed& query_) -> boxed {
  return [&]() -> boxed {
    const auto query = unbox<string>(query_);
    ButtonRegistry* buttonRegistry = ButtonRegistry::ptr();
    auto button = buttonRegistry->find_button(query);
    return box<ButtonHandle>(button);
  };
};

exports["isButtonDown"] = [](const boxed& window_) -> boxed {
  return [&](const boxed& button_) -> boxed {
    return [&]() -> boxed {
      const auto& window = unbox<WindowFramework*>(window_);
      const auto& button = unbox<ButtonHandle>(button_);
      MouseWatcher* mouseWatcher = DCAST(MouseWatcher, window->get_mouse().node());
      return box<bool>(mouseWatcher->is_button_down(button));
    };
  };
};

exports["findNodePath"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& query_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto query = unbox<string>(query_);
      auto newNodePath = nodePath.find(query);
      return box<NodePath>(newNodePath);
    };
  };
};

exports["exposeCharacterJoint"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& query_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto query = unbox<string>(query_);
      auto jointNodePath = NodePath(query);
      PT(Character) character = DCAST(Character, nodePath.find("**/+Character").node());
      PT(CharacterJoint) characterJoint = character->find_joint(query);
      characterJoint->add_net_transform(jointNodePath.node());
      nodePath.attach_new_node(jointNodePath.node());
      return box<NodePath>(jointNodePath);
    };
  };
};

exports["sin"] = [](const boxed& d_) -> boxed {
  const auto d = unbox<double>(d_);
  double r = d * (3.14 / 180.0);
  return box<double>(sin(r));
};

exports["cos"] = [](const boxed& d_) -> boxed {
  const auto d = unbox<double>(d_);
  double r = d * (3.14 / 180.0);
  return box<double>(cos(r));
};

exports["getRealTime"] = []() -> boxed {
  return box<double>(globalClock->get_real_time());
};

exports["getRandomNumber"] = [](const boxed& range_) -> boxed {
  return [&]() -> boxed {
    const auto range = unbox<double>(range_);
    return box<double>(Randomizer().random_real(range));
  };
};

exports["updateAudioManager"] = []() -> boxed {
  audioManager->update();
  return boxed();
};

exports["createEmptyNodePath"] = [](const boxed& name_) -> boxed {
  return [&]() -> boxed {
    const auto name = unbox<string>(name_);
    auto nodePath = NodePath(name);
    return box<NodePath>(nodePath);
  };
};

exports["removeNodePath"] = [](const boxed& nodePath_) -> boxed {
  return [&]() -> boxed {
    auto nodePath = unbox<NodePath>(nodePath_);
    nodePath.remove_node();
    return boxed();
  };
};

exports["addScreenText"] = [](const boxed& window_) -> boxed {
  return [&](const boxed& name_) -> boxed {
    return [&](const boxed& text_) -> boxed {
      return [&]() -> boxed {
        const auto& window = unbox<WindowFramework*>(window_);
        const auto name = unbox<string>(name_);
        const auto text = unbox<string>(text_);
        PT(TextNode) textNode = new TextNode(name);
        textNode->set_text(text);
        auto nodePath = window->get_aspect_2d().attach_new_node(textNode);
        return box<NodePath>(nodePath);
      };
    };
  };
};

exports["setText"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& text_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto text = unbox<string>(text_);
      DCAST(TextNode, nodePath.node())->set_text(text);
      return boxed();
    };
  };
};

exports["setFont"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& font_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto& font = unbox<PT(TextFont)>(font_);
      DCAST(TextNode, nodePath.node())->set_font(font);
      return boxed();
    };
  };
};

exports["setTextAlignment"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& alignment_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto alignment = unbox<TextProperties::Alignment>(alignment_);
      DCAST(TextNode, nodePath.node())->set_align(alignment);
      return boxed();
    };
  };
};

exports["setTextCardColor"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& r_) -> boxed {
    return [&](const boxed& g_) -> boxed {
      return [&](const boxed& b_) -> boxed {
        return [&](const boxed& a_) -> boxed {
          return [&]() -> boxed {
            auto nodePath = unbox<NodePath>(nodePath_);
            const auto r = unbox<double>(r_);
            const auto g = unbox<double>(g_);
            const auto b = unbox<double>(b_);
            const auto a = unbox<double>(a_);
            DCAST(TextNode, nodePath.node())->set_card_color(r, g, b, a);
            return boxed();
          };
        };
      };
    };
  };
};

exports["setTextCardMargin"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& l_) -> boxed {
    return [&](const boxed& r_) -> boxed {
      return [&](const boxed& b_) -> boxed {
        return [&](const boxed& t_) -> boxed {
          return [&]() -> boxed {
            auto nodePath = unbox<NodePath>(nodePath_);
            const auto l = unbox<double>(l_);
            const auto r = unbox<double>(r_);
            const auto b = unbox<double>(b_);
            const auto t = unbox<double>(t_);
            DCAST(TextNode, nodePath.node())->set_card_as_margin(l, r, b, t);
            return boxed();
          };
        };
      };
    };
  };
};

exports["setTextCardDecal"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& isDecal_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto isDecal = unbox<bool>(isDecal_);
      DCAST(TextNode, nodePath.node())->set_card_decal(isDecal);
      return boxed();
    };
  };
};

exports["showNodePath"] = [](const boxed& nodePath_) -> boxed {
  return [&](const boxed& show_) -> boxed {
    return [&]() -> boxed {
      auto nodePath = unbox<NodePath>(nodePath_);
      const auto show = unbox<bool>(show_);
      if (show) {
        nodePath.show();
      } else {
        nodePath.hide();
      }
      return boxed();
    };
  };
};

exports["textAlignCenter"] = []() -> boxed {
  return box<TextProperties::Alignment>(TextNode::A_center);
};

exports["getEnv"] = [](const boxed& key_) -> boxed {
  return [&]() -> boxed {
    const auto key = unbox<string>(key_);
    const auto value = std::getenv(key.c_str());
    if (value) {
      return std::string(value);
    } else {
      return "";
    }
  };
};

exports["getStringLength"] = [](const boxed& s_) -> boxed {
  return unbox<string>(s_).length();
};

exports["toOsGenericFilename"] = [](const boxed& s_) -> boxed {
  return Filename(unbox<string>(s_).c_str()).to_os_generic();
};

FOREIGN_END
