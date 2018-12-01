{-
  (C) 2018 David Lettier
  lettier.com
-}

module Panda3d where

import Prelude
import Effect

foreign import data Framework :: Type
foreign import data Window :: Type
foreign import data NodePath :: Type
foreign import data PandaNode :: Type
foreign import data DisplayRegion :: Type
foreign import data Lens :: Type
foreign import data AnimControlCollection :: Type
foreign import data Shader :: Type
foreign import data Button :: Type
foreign import data Sound :: Type
foreign import data Font :: Type
foreign import data TextAlignment :: Type

foreign import withFramework :: (Framework -> Effect Unit) -> Effect Unit

foreign import withWindow :: Framework -> (Window -> Effect Unit) -> Effect Unit

foreign import setWindowTitle :: Framework -> String -> Effect Unit

foreign import getCamera :: Window -> Effect NodePath

foreign import getDisplayRegion :: Window -> Effect DisplayRegion

foreign import getModels :: Framework -> Effect NodePath

foreign import getRender :: Window -> Effect NodePath

-- TODO: LensNode Typeclass
foreign import getLens :: NodePath -> Effect Lens

foreign import getSound :: String -> Effect Sound

foreign import playSound :: Sound -> Effect Unit
foreign import stopSound :: Sound -> Effect Unit

foreign import setSoundVolume :: Sound -> Number  -> Effect Unit
foreign import setSoundLoop   :: Sound -> Boolean -> Effect Unit

foreign import loopAnimations :: Window -> Int -> Effect Unit

foreign import loadModel :: Window -> NodePath -> String -> Effect NodePath

foreign import loadFont :: String -> Effect Font

foreign import loadAnimations :: NodePath -> Effect AnimControlCollection

foreign import loadShader :: String -> Effect Shader

foreign import loopAnimation :: AnimControlCollection -> String -> Boolean -> Effect Unit

foreign import stopAnimation :: AnimControlCollection -> String -> Boolean -> Effect Unit

foreign import playAnimation :: AnimControlCollection -> String -> Effect Unit

foreign import createAmbientLight :: String -> Number -> Number -> Number -> Effect PandaNode

foreign import createPointLight       :: String -> Boolean -> Effect PandaNode
foreign import createDirectionalLight :: String -> Boolean -> Effect PandaNode
foreign import createSpotlight        :: String -> Boolean -> Effect PandaNode

type ParentNodePath = NodePath
type ChildNodePath  = NodePath

foreign import reparentTo :: ParentNodePath -> ChildNodePath -> Effect Unit

foreign import attachNewNode :: ParentNodePath -> PandaNode -> Effect ChildNodePath

foreign import setScale    :: NodePath -> Number -> Number -> Number -> Effect Unit
foreign import setPosition :: NodePath -> Number -> Number -> Number -> Effect Unit
foreign import setHpr      :: NodePath -> Number -> Number -> Number -> Effect Unit

foreign import setLight :: NodePath -> NodePath -> Effect Unit

foreign import setFov :: Lens -> Number -> Effect Unit

foreign import setShaderAuto :: NodePath -> Effect Unit

foreign import setShader :: Shader -> NodePath -> Effect Unit

type InputNodePath = NodePath

foreign import setShaderInputNodePath :: NodePath -> String -> InputNodePath -> Effect Unit

foreign import setColor :: NodePath -> Number -> Number -> Number -> Effect Unit

foreign import setAttenutation :: NodePath -> Number -> Number -> Number -> Effect Unit

foreign import setClearColor :: DisplayRegion -> Number -> Number -> Number -> Effect Unit

foreign import setLightRampAttrib :: NodePath -> Number -> Number -> Number -> Number -> Effect Unit

foreign import getPosition :: NodePath -> Effect { x :: Number, y :: Number, z :: Number }
foreign import getHpr      :: NodePath -> Effect { h :: Number, p :: Number, r :: Number }

foreign import lookAt :: NodePath -> NodePath -> Effect Unit

foreign import addGenericAsyncTask :: String -> Effect Unit -> Effect Unit

foreign import addCollisionSphere :: NodePath -> String -> Number -> Number -> Number -> Number -> Boolean -> Effect NodePath

foreign import addCollisionBox
  ::  NodePath
  ->  String
  ->  Number
  ->  Number
  ->  Number
  ->  Number
  ->  Number
  ->  Number
  ->  Boolean
  ->  Effect NodePath

type ColliderNodePath = NodePath
type TargetNodePath   = NodePath

foreign import addColliderToTraverserAndPusher
  ::  TargetNodePath
  ->  ColliderNodePath
  ->  Effect Unit

foreign import traverseCollisionTraverser :: NodePath -> Effect Unit

foreign import getRealTime :: Effect Number

foreign import findButton :: String -> Effect Button

foreign import isButtonDown :: Window -> Button -> Effect Boolean

foreign import findNodePath :: NodePath -> String -> Effect NodePath

foreign import exposeCharacterJoint :: NodePath -> String -> Effect NodePath

foreign import sin :: Number -> Number
foreign import cos :: Number -> Number

foreign import getRandomNumber :: Number -> Effect Number

foreign import updateAudioManager :: Effect Unit

foreign import createEmptyNodePath :: String -> Effect NodePath

foreign import removeNodePath :: NodePath -> Effect Unit

-- TODO: newtype
type NodePathName = String
type TextNodeText = String

foreign import addScreenText :: Window -> NodePathName -> TextNodeText -> Effect NodePath

foreign import setText :: NodePath -> TextNodeText -> Effect Unit

foreign import setFont :: NodePath -> Font -> Effect Unit

foreign import setTextAlignment :: NodePath -> TextAlignment -> Effect Unit

foreign import setTextCardColor  :: NodePath -> Number -> Number -> Number -> Number -> Effect Unit
foreign import setTextCardMargin :: NodePath -> Number -> Number -> Number -> Number -> Effect Unit

foreign import setTextCardDecal :: NodePath -> Boolean -> Effect Unit

foreign import showNodePath :: NodePath -> Boolean -> Effect Unit

foreign import textAlignCenter :: TextAlignment
