{-
  Lambda Lantern
  (C) 2018 David Lettier
  lettier.com
-}

module Main where

import Prelude
import Data.Newtype
import Data.Time.Duration
import Data.DateTime.Instant
import Effect
import Effect.Console
import Effect.Now
import Effect.Timer
import Math (pow)
import FRP.Behavior
import FRP.Event
import FRP.Event.Time

import Panda3d

data GameState
  = MainMenu
  | Level
  | Ending

data MainMenuState
  = MainMenuStateAnimating
  | MainMenuStateWaiting
  | MainMenuStateCleanUp
  | MainMenuStateDone

data LevelState
  = LevelStateIntro
  | LevelStatePlaying
  | LevelStateFailure
  | LevelStateSuccess
  | LevelStateCleanUp
  | LevelStateDone

main :: Effect Unit
main =
  withFramework
    (\ framework -> do
      setWindowTitle framework "Lambda Lantern by David Lettier"
      withWindow
        framework
        (\ window -> do
          displayRegion <- getDisplayRegion window
          setClearColor displayRegion 0.125 0.122 0.184

          camera <- getCamera window
          render <- getRender window
          models <- getModels framework

          cameraLens <- getLens camera

          font <- loadFont "./assets/fonts/gloria-hallelujah.ttf"

          wButton     <- findButton "w"
          aButton     <- findButton "a"
          sButton     <- findButton "s"
          dButton     <- findButton "d"
          oneButton   <- findButton "1"
          twoButton   <- findButton "2"
          threeButton <- findButton "3"
          spaceButton <- findButton "space"

          mainMenuMusic <- getSound "./assets/music/big-eyes.ogg"
          levelMusic    <- getSound "./assets/music/night-in-the-castle.ogg"
          endingMusic   <- getSound "./assets/music/magic-in-the-garden.ogg"

          evilLaugh     <- getSound "./assets/sounds/evil-laugh.ogg"
          levelComplete <- getSound "./assets/sounds/level-complete.ogg"
          chicken       <- getSound "./assets/sounds/chicken.ogg"
          twinkle       <- getSound "./assets/sounds/twinkle.ogg"
          stoneDragging <- getSound "./assets/sounds/stone-dragging.ogg"
          doorOpen      <- getSound "./assets/sounds/door-open.ogg"

          gameStateEvent <- create

          void $
            subscribe
              gameStateEvent.event
              (\ gameState ->
                case gameState of
                  MainMenu -> do
                    sceneRoot <- createEmptyNodePath "scene-root"
                    reparentTo render sceneRoot

                    setFov cameraLens 35.0
                    setPosition camera (-1.0) (-20.0) (-5.0)

                    pressSText <-
                      addScreenText window "press-s-text" "Press S..."
                    setScale pressSText (0.15) (0.15) (0.15)
                    setPosition pressSText (-0.3) (0.0) (-0.5)
                    setFont pressSText font
                    setTextAlignment pressSText textAlignCenter
                    showNodePath pressSText false

                    ambientLight' <- createAmbientLight "ambientLight" 0.203 0.198 0.301
                    ambientLight <- attachNewNode sceneRoot ambientLight'
                    setLight sceneRoot ambientLight

                    light' <- createPointLight "light" false
                    light <- attachNewNode sceneRoot light'
                    setPosition light (-6.6) (-3.5) (-2.9)
                    setColor light 0.2 0.2 0.2
                    setLight sceneRoot light

                    light1' <- createPointLight "light1" true
                    light1 <- attachNewNode sceneRoot light1'
                    setPosition light1 (2.72) (-0.668) (1.41)
                    setColor light1 (0.1) (0.38) (0.51)
                    setAttenutation light1 0.0 0.0 0.1
                    setLight sceneRoot light1

                    logo <- loadModel window models "./assets/eggs/logo/logo"
                    reparentTo sceneRoot logo

                    light2' <- createPointLight "light2" false
                    light2 <- attachNewNode sceneRoot light2'
                    setColor light2 (1.0) (0.15) (0.29)
                    setAttenutation light2 0.0 0.0 0.01
                    setLight sceneRoot light2

                    logoAnimations <- loadAnimations logo
                    loopAnimation logoAnimations "swing" true

                    lanternJoint <- exposeCharacterJoint logo "bone"
                    reparentTo lanternJoint light2
                    setPosition light2 (0.0) (3.0) (0.0)

                    cancelLighFlickerLoop <-
                      subscribe
                        (interval 100)
                        (\ _ -> do
                          modifier1 <- getRandomNumber 0.1
                          setColor light1 (0.1 - modifier1) (0.38 - modifier1) (0.51 - modifier1)
                        )

                    setSoundVolume mainMenuMusic 0.1
                    setSoundLoop   mainMenuMusic true
                    playSound      mainMenuMusic

                    mainMenuStateEvent <- create

                    cancelMainMenuLoop <-
                      subscribe
                        (interval 16 *> mainMenuStateEvent.event)
                        (\ mainMenuState ->
                          case mainMenuState of
                            MainMenuStateAnimating -> do
                              cameraPosition <- getPosition camera
                              if cameraPosition.z < -2.0
                                then
                                  setPosition
                                    camera
                                    cameraPosition.x
                                    cameraPosition.y
                                    (cameraPosition.z + 0.01)
                                else
                                  mainMenuStateEvent.push MainMenuStateWaiting

                            MainMenuStateWaiting -> do
                              showNodePath pressSText true

                              sIsDown <- isButtonDown window sButton
                              when sIsDown $ do
                                mainMenuStateEvent.push MainMenuStateCleanUp

                            MainMenuStateCleanUp -> do
                              cancelLighFlickerLoop

                              removeNodePath sceneRoot
                              removeNodePath pressSText

                              stopSound mainMenuMusic

                              mainMenuStateEvent.push MainMenuStateDone
                              gameStateEvent.push Level

                            MainMenuStateDone -> pure unit
                        )

                    mainMenuStateEvent.push MainMenuStateAnimating

                  Level -> do
                    sceneRoot <- createEmptyNodePath "scene-root"
                    reparentTo render sceneRoot

                    setFov cameraLens 55.0
                    setPosition camera (168.0) (-165.0) (108.0)

                    room0 <- loadModel window models "./assets/eggs/room-0/room-0"
                    reparentTo sceneRoot room0

                    ambientLight' <- createAmbientLight "ambientLight" 0.125 0.122 0.184
                    ambientLight <- attachNewNode sceneRoot ambientLight'
                    setLight sceneRoot ambientLight

                    light' <- createPointLight "light" false
                    light <- attachNewNode sceneRoot light'
                    setPosition light (44.0) (-64.0) (10.0)
                    setColor light (0.2) (0.2) (0.2)
                    setLight sceneRoot light

                    light1' <- createPointLight "light1" true
                    light1 <- attachNewNode sceneRoot light1'
                    setPosition light1 (43.0) (27.0) (10.0)
                    setColor light1 (1.0) (0.737) (0.498)
                    setLight sceneRoot light1

                    light2' <- createPointLight "light2" true
                    light2 <- attachNewNode sceneRoot light2'
                    setPosition light2 (-33.0) (-38.0) (10.0)
                    setColor light2 (0.38) (0.737) (0.663)
                    setLight sceneRoot light2

                    mainCharacter <- loadModel window sceneRoot "./assets/eggs/main-character/main-character"
                    setPosition mainCharacter 0.0 0.0 25.0
                    lookAt mainCharacter camera

                    mainCharacterCollisionSphere <-
                      addCollisionSphere
                        mainCharacter
                        "main-character-collision-sphere"
                        0.0
                        0.0
                        0.0
                        15.0
                        false

                    crate <- findNodePath room0 "crate"
                    reparentTo sceneRoot crate
                    _ <-
                      addCollisionBox
                        crate
                        "crate-collision-box"
                        0.0
                        0.0
                        0.0
                        8.0
                        8.0
                        8.0
                        false

                    door <- findNodePath room0 "door"
                    reparentTo sceneRoot door
                    setHpr door 0.0 0.0 0.0
                    _ <-
                      addCollisionBox
                        door
                        "door-collision-box"
                        17.0
                        0.0
                        0.0
                        17.0
                        6.0
                        25.0
                        false

                    addColliderToTraverserAndPusher
                      mainCharacter
                      mainCharacterCollisionSphere

                    _ <-
                      addCollisionBox
                        sceneRoot
                        "east-wall-collision-box"
                        (-55.0)
                        (-5.0)
                        (34.0)
                        5.0
                        70.0
                        30.0
                        false

                    _ <-
                      addCollisionBox
                        sceneRoot
                        "south-wall-collision-box"
                        (0.0)
                        (-75.0)
                        (34.0)
                        70.0
                        5.0
                        30.0
                        false

                    _ <-
                      addCollisionBox
                        sceneRoot
                        "west-wall-collision-box"
                        (60.0)
                        (-5.0)
                        (34.0)
                        5.0
                        70.0
                        30.0
                        false

                    _ <-
                      addCollisionBox
                        sceneRoot
                        "north-left-wall-collision-box"
                        (-34.0)
                        (50.0)
                        (34.0)
                        15.0
                        5.0
                        30.0
                        false

                    _ <-
                      addCollisionBox
                        sceneRoot
                        "north-right-wall-collision-box"
                        (34.0)
                        (50.0)
                        (34.0)
                        15.0
                        5.0
                        30.0
                        false

                    mainCharacterAnimations <- loadAnimations mainCharacter

                    loopAnimation mainCharacterAnimations "hover" true

                    setSoundVolume levelMusic 0.1
                    setSoundLoop   levelMusic true
                    playSound      levelMusic

                    let totalSeconds = 60
                    timerText <- addScreenText window "timer-text" (show totalSeconds)
                    setFont timerText font
                    setScale timerText 0.5 0.5 0.5
                    setPosition timerText (-0.2) (0.0) (0.5)
                    showNodePath timerText false

                    youLostText <-
                      addScreenText window "you-lost-text" "You lost! :("
                    setScale youLostText (0.16) (0.15) (0.15)
                    setPosition youLostText (-0.4) (0.0) (-0.5)
                    setFont youLostText font
                    setTextAlignment youLostText textAlignCenter
                    showNodePath youLostText false

                    youWonText <-
                      addScreenText window "you-won-text" "You won! :)"
                    setScale youWonText (0.16) (0.15) (0.15)
                    setPosition youWonText (-0.4) (0.0) (-0.5)
                    setFont youWonText font
                    setTextAlignment youWonText textAlignCenter
                    showNodePath youWonText false

                    let puzzleText'
                          =   "You examine the crate and there appears to be a chicken inside?!\n"
                          <>  "As you try to get the chicken out, you notice the stone underneath\n"
                          <>  "springs up and down a bit.\n"
                          <>  "Curiously, every time you push, the door also jiggles behind you.\n"
                          <>  "If you can make the crate heavy enough, I bet the door will pop open!"
                          <>  "\n\n"
                          <>  "Crate(Boulder) = ___ Crate(Chicken)"
                          <>  "\n\n"
                          <>  "Which incantation fills out the blank? Press 1, 2, or 3."
                          <>  "\n\n"
                          <>  "1) fmap ((Feathers -> Boulder) . (Chicken -> Feathers))\n"
                          <>  "2) foldl (Feathers -> Chicken -> Feathers) Feathers\n"
                          <>  "3) flip (>>=) (Boulder -> Crate(Chicken))\n"

                    puzzleText <- addScreenText window "intro-text" puzzleText'
                    setFont puzzleText font
                    setScale puzzleText 0.06 0.06 0.06
                    setPosition puzzleText (-1.0) (0.0) (0.7)
                    setTextCardColor puzzleText 0.1 0.1 0.1 0.8
                    setTextCardMargin puzzleText 0.8 0.8 0.8 0.8
                    setTextCardDecal puzzleText true
                    showNodePath puzzleText false

                    puzzleTextShownEvent <- create

                    levelStateEvent <- create

                    cancelLighFlickerLoop <-
                      subscribe
                        (interval 100)
                        (\ _ -> do
                          modifier1 <- getRandomNumber 0.1
                          modifier2 <- getRandomNumber 0.1
                          setColor light1 (1.0   - modifier1) (0.504 - modifier1) (0.213 - modifier1)
                          setColor light2 (0.120 - modifier2) (0.505 - modifier2) (0.395 - modifier2)
                        )

                    crateAnimationEvent <- create
                    doorAnimationEvent  <- create

                    let crateLoweredHeight = 1.0
                    let doorOpenedHeading  = 120.0

                    cancelPlayingStateLoop <-
                      subscribe
                        (interval 16 *> levelStateEvent.event)
                        (\ levelState -> do
                          case levelState of
                            LevelStatePlaying -> do
                              wIsDown     <- isButtonDown window wButton
                              sIsDown     <- isButtonDown window sButton
                              aIsDown     <- isButtonDown window aButton
                              dIsDown     <- isButtonDown window dButton
                              isSpaceDown <- isButtonDown window spaceButton

                              hpr <- getHpr mainCharacter

                              setHpr mainCharacter hpr.h (0.0) (0.0)

                              when wIsDown $ do
                                setHpr mainCharacter (0.0) (0.0) (0.0)
                                setHpr mainCharacter (180.0) (10.0) (0.0)
                                position <- getPosition mainCharacter
                                setPosition mainCharacter position.x (position.y + 1.0) (position.z)
                              when aIsDown $ do
                                setHpr mainCharacter (0.0) (0.0) (0.0)
                                setHpr mainCharacter (-90.0) (10.0) (0.0)
                                position <- getPosition mainCharacter
                                setPosition mainCharacter (position.x - 1.0) (position.y) (position.z)
                              when sIsDown $ do
                                setHpr mainCharacter (0.0) (10.0) (0.0)
                                position <- getPosition mainCharacter
                                setPosition mainCharacter (position.x) (position.y - 1.0) (position.z)
                              when dIsDown $ do
                                setHpr mainCharacter (0.0) (0.0) (0.0)
                                setHpr mainCharacter (90.0) (10.0) (0.0)
                                position <- getPosition mainCharacter
                                setPosition mainCharacter (position.x + 1.0) (position.y) (position.z)

                              mainCharacterPosition <- getPosition mainCharacter
                              cratePosition <- getPosition crate

                              let diff =
                                    { x: mainCharacterPosition.x - cratePosition.x
                                    , y: mainCharacterPosition.y - cratePosition.y
                                    , z: mainCharacterPosition.z - cratePosition.z
                                    }

                              let distance =
                                    pow
                                      (   pow (mainCharacterPosition.x - cratePosition.x) 2.0
                                      +   pow (mainCharacterPosition.y - cratePosition.y) 2.0
                                      )
                                      0.5

                              cratePosition <- getPosition crate

                              when (distance <= 25.0) $ do
                                when (isSpaceDown && cratePosition.z >= crateLoweredHeight) $ do
                                  showNodePath puzzleText true
                                  setSoundVolume chicken 0.1
                                  playSound chicken
                                  puzzleTextShownEvent.push true

                              when (mainCharacterPosition.y >= 56.0) $
                                levelStateEvent.push LevelStateSuccess

                            _ -> pure unit
                        )

                    cancelPuzzleTextLoop <-
                      subscribe
                        (interval 16 *> puzzleTextShownEvent.event)
                        (\ puzzleTextShown -> do
                          when puzzleTextShown $ do
                            oneIsDown   <- isButtonDown window oneButton
                            twoIsDown   <- isButtonDown window twoButton
                            threeIsDown <- isButtonDown window threeButton

                            when oneIsDown $ do
                              setSoundVolume twinkle 0.1
                              playSound twinkle
                              showNodePath puzzleText false
                              crateAnimationEvent.push true
                              setSoundVolume stoneDragging 0.2
                              playSound stoneDragging
                            when twoIsDown $
                              levelStateEvent.push LevelStateFailure
                            when threeIsDown $
                              levelStateEvent.push LevelStateFailure
                        )

                    cancelCrateAnimation <-
                      subscribe
                        (interval 16 *> crateAnimationEvent.event)
                        (\ crateAnimation -> do
                          when crateAnimation $ do
                            cratePosition <- getPosition crate
                            when (cratePosition.z >= crateLoweredHeight) $ do
                              setPosition crate cratePosition.x cratePosition.y (cratePosition.z - 0.07)
                              when (cratePosition.z - 0.07 < crateLoweredHeight) $ do
                                setSoundVolume doorOpen 0.1
                                playSound doorOpen
                                doorAnimationEvent.push true
                        )

                    cancelDoorAnimation <-
                      subscribe
                        (interval 16 *> doorAnimationEvent.event)
                        (\ doorAnimation -> do
                          when doorAnimation $ do
                            doorHpr <- getHpr door
                            when (doorHpr.h <= doorOpenedHeading) $
                              setHpr door (doorHpr.h + 1.0) doorHpr.p doorHpr.r
                        )

                    secondsTickerEvent <- create
                    cancelSecondsPassedLoop <-
                      subscribe
                        (sampleOn_ secondsTickerEvent.event (interval 1000))
                        (\ secondsPassed -> do
                          when (secondsPassed <= (totalSeconds + 1)) $ do
                            let secondsLeft = totalSeconds - secondsPassed
                            if secondsLeft < 0
                              then do
                                levelStateEvent.push LevelStateFailure
                              else do
                                setText timerText (show secondsLeft)
                            secondsTickerEvent.push (secondsPassed + 1)
                        )

                    _ <-
                      subscribe
                      levelStateEvent.event
                      (\ levelState ->
                        case levelState of
                          LevelStateIntro -> do
                            let introText'
                                  =   "The evil sorcerer State has trapped you in their dungeon of complexity.\n"
                                  <>  "Good thing you have the Lambda Lantern!\n"
                                  <>  "Assemble the right incantation and escape State's dungeon but hurry—\n"
                                  <>  "Lambda Lantern's light is about to run out!"
                                  <>  "\n\n"
                                  <>  "Press W, A, S, and/or D to move around.\n"
                                  <>  "If something is context sensitive, press the space bar\n"
                                  <>  "to interact with it."
                                  <>  "\n\n"
                                  <>  "Okay press space to start! :D"

                            introText <- addScreenText window "intro-text" introText'
                            setFont introText font
                            setScale introText 0.06 0.06 0.06
                            setPosition introText (-1.07) (0.0) (0.5)
                            setTextCardColor introText 0.1 0.1 0.1 0.8
                            setTextCardMargin introText 0.8 0.8 0.8 0.8
                            setTextCardDecal introText true

                            setSoundVolume evilLaugh 0.1
                            playSound evilLaugh

                            leavingIntroEvent <- create

                            cancelIntroLoop <-
                              subscribe
                                (sampleOn_ leavingIntroEvent.event (interval 16))
                                (\ leavingIntro -> do
                                  unless leavingIntro $ do
                                    isSpaceDown <- isButtonDown window spaceButton
                                    when isSpaceDown $ do
                                      removeNodePath introText
                                      leavingIntroEvent.push true
                                      levelStateEvent.push LevelStatePlaying
                                )

                            leavingIntroEvent.push false

                            void $
                              subscribe
                                leavingIntroEvent.event
                                (\ leavingIntro -> do
                                  when leavingIntro $ do
                                    -- TODO:
                                    -- https://github.com/paf31/purescript-event/blob/af8b8d2e381f423c0b5e739e92668e392e77efce/src/FRP/Event.purs
                                    -- https://github.com/purescript-contrib/purescript-unsafe-reference/blob/6aa19a5c1f4520f77814e8dc84d5c3d54fbb203e/src/Unsafe/Reference.js#L3
                                    cancelIntroLoop
                                    pure unit
                                )

                          LevelStatePlaying -> do
                            showNodePath timerText true

                            secondsTickerEvent.push 0

                          LevelStateFailure -> do
                            showNodePath timerText false
                            showNodePath puzzleText false
                            showNodePath youLostText true

                            setColor light  0.0 0.0 0.0
                            setColor light1 0.0 0.0 0.0
                            setColor light2 0.0 0.0 0.0

                            setSoundVolume evilLaugh 0.1
                            playSound evilLaugh

                            cancelLighFlickerLoop
                            stopSound levelMusic

                            void $
                              setTimeout
                                2000
                                (do levelStateEvent.push LevelStateCleanUp)

                          LevelStateSuccess -> do
                            showNodePath timerText false
                            showNodePath puzzleText false
                            showNodePath youWonText true

                            stopSound levelMusic

                            setSoundVolume levelComplete 0.1
                            playSound levelComplete

                            void $
                              setTimeout
                                2000
                                (do levelStateEvent.push LevelStateCleanUp)

                          LevelStateCleanUp -> do
                            cancelSecondsPassedLoop
                            cancelPlayingStateLoop
                            cancelCrateAnimation
                            cancelDoorAnimation
                            cancelPuzzleTextLoop

                            removeNodePath sceneRoot
                            removeNodePath timerText
                            removeNodePath puzzleText
                            removeNodePath youLostText
                            removeNodePath youWonText

                            levelStateEvent.push LevelStateDone
                            gameStateEvent.push Ending

                          LevelStateDone -> pure unit
                      )

                    levelStateEvent.push LevelStateIntro

                    pure unit

                  Ending -> do
                    setSoundVolume endingMusic 0.1
                    playSound endingMusic

                    let endingText'
                          =   "Lambda Lantern was created by me, David Lettier. Visit lettier.com and\n"
                          <>  "follow @lettier on Twitter if you'd like."
                          <>  "\n\n"
                          <>  "Keep an eye on Lambda Lantern. New releases will be bigger and better\n"
                          <>  "with more levels and more interesting puzzles!"
                          <>  "\n\n"
                          <>  "Special thanks to freepd.com for the music, freesound.com for the\n"
                          <>  "sounds, and Kimberly Geswein for the font."
                          <>  "\n\n"
                          <>  "Thanks for playing! :D"

                    endingText <- addScreenText window "intro-text" endingText'
                    setFont endingText font
                    setScale endingText 0.06 0.06 0.06
                    setPosition endingText (-1.1) (0.0) (0.6)
              )

          _ <-
            subscribe
              (interval 16)
              (\ _ -> do
                updateAudioManager
                traverseCollisionTraverser render
              )

          gameStateEvent.push MainMenu

          setShaderAuto render
        )
    )
