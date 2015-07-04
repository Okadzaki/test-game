#include "HelloWorldScene.h"

USING_NS_CC;

enum class PhysicsCategory {
  None = 0,
  Monster = (1 << 0),    // 1
  Projectile = (1 << 1), // 2
  All = PhysicsCategory::Monster | PhysicsCategory::Projectile // 3
};


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2(0,0));
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    



    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object

    auto background =   DrawNode::create();
    background->drawSolidRect(origin, visibleSize, Color4F::GRAY);

    this->addChild(background);

    player = Sprite::create("player.png");
    player->setPosition(Vec2(visibleSize.width * 0.1, visibleSize.height * 0.5));
    this->addChild(player);



    srand((unsigned int)time(nullptr));
    this->schedule(schedule_selector(HelloWorld::addMonster), 1.5);


    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, player);


    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}


void HelloWorld::addMonster(float dt){
    auto monster = Sprite::create("monster.png");

    // 1
    auto monsterContentSize = monster->getContentSize();
    auto monsterSize = monster->getContentSize();
    auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width , monsterSize.height),
                                              PhysicsMaterial(0.1f, 1.0f, 0.0f));
    auto selfContentSize = this->getContentSize();
    int minY = monsterContentSize.height/2;
    int maxY = selfContentSize.height - monsterContentSize.height/2;
    int rangeY = maxY - minY;
    int randomY = (rand() % rangeY) + minY;

    monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width/2, randomY));
    this->addChild(monster);

    // 2
    physicsBody->setDynamic(true);
    int minDuration = 2.0;
    int maxDuration = 4.0;
    int rangeDuration = maxDuration - minDuration;
    int randomDuration = (rand() % rangeDuration) + minDuration;

    // 3
    physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);
    physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
    physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);
    auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width/2, randomY));
    auto actionRemove = RemoveSelf::create();
    monster->setPhysicsBody(physicsBody);
    monster->runAction(Sequence::create(actionMove,actionRemove, nullptr));
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event) {
  // 1  - Just an example for how to get the  _player object
  //auto node = unused_event->getCurrentTarget();

  // 2
  Vec2 touchLocation = touch->getLocation();
  Vec2 offset = touchLocation - player->getPosition();

  // 3
  if (offset.x < 0) {
    return true;
  }

  // 4
  auto projectile = Sprite::create("projectile.png");
  projectile->setPosition(player->getPosition());
  this->addChild(projectile);

  auto projectileSize = projectile->getContentSize();
  auto physicsBody = PhysicsBody::createCircle(projectileSize.width/2 );
  physicsBody->setDynamic(true);
  physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
  physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
  physicsBody->setContactTestBitmask((int)PhysicsCategory::Monster);
  projectile->setPhysicsBody(physicsBody);

  // 5
  offset.normalize();
  auto shootAmount = offset * 1000;

  // 6
  auto realDest = shootAmount + projectile->getPosition();

  // 7
  auto actionMove = MoveTo::create(2.0f, realDest);
  auto actionRemove = RemoveSelf::create();
  projectile->runAction(Sequence::create(actionMove,actionRemove, nullptr));

  return true;
}

bool HelloWorld::onContactBegan(PhysicsContact &contact) {
  auto nodeA = contact.getShapeA()->getBody()->getNode();
  auto nodeB = contact.getShapeB()->getBody()->getNode();

  nodeA->removeFromParent();
  nodeB->removeFromParent();
  return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
