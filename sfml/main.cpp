#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <functional>



const float GRAVITY = 560.0f;
const float BALL_RADIUS = 10.0f;
const float BALL_DENSITY = 1.0f;
const float STORM_FORCE = 2000.0f;
const float BALL_GENERATION_DELAY = 0.1f;
const float GRID_SIZE = 50.0f;

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

namespace std {
    template <typename T1, typename T2>
    struct hash<std::pair<T1, T2>> {
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
}

class SpatialPartition {
public:
    SpatialPartition(float cellSize) : cellSize(cellSize) {}

    void addBall(std::shared_ptr<Ball> ball) {
        int x = static_cast<int>(ball->shape.getPosition().x / cellSize);
        int y = static_cast<int>(ball->shape.getPosition().y / cellSize);
        grid[std::make_pair(x, y)].push_back(ball);
    }

    const std::vector<std::shared_ptr<Ball>>& getBallsInCell(int x, int y) const {
        auto it = grid.find(std::make_pair(x, y));
        if (it != grid.end()) {
            return it->second;
        } else {
            static std::vector<std::shared_ptr<Ball>> empty;
            return empty;
        }
    }

    void clear() {
        grid.clear();
    }

private:
    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Ball>>> grid;
};

void handleBallCollision(Ball& ball1, Ball& ball2) {
    sf::Vector2f delta = ball1.shape.getPosition() - ball2.shape.getPosition();
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float combinedRadius = 2 * BALL_RADIUS;

    if (distance < combinedRadius) {
        sf::Vector2f normal = delta / distance;
        float relativeVelocity = (ball1.velocity - ball2.velocity).x * normal.x + (ball1.velocity - ball2.velocity).y * normal.y;

        if (relativeVelocity < 0) {
            float coefficientOfRestitution = 0.8f;
            float impulseMagnitude = (1 + coefficientOfRestitution) * relativeVelocity / (2 / BALL_DENSITY);
            ball1.velocity -= impulseMagnitude / BALL_DENSITY * normal;
            ball2.velocity += impulseMagnitude / BALL_DENSITY * normal;
        }
    }
}

void updateBalls(std::vector<std::shared_ptr<Ball>>& balls, float deltaTime, bool stormActive, sf::Vector2i mousePosition, sf::Vector2u windowSize, SpatialPartition& partition) {
    partition.clear();
    for (auto& ball : balls) {
        ball->velocity.y += GRAVITY * deltaTime;
        sf::Vector2f movement = ball->velocity * deltaTime;
        sf::Vector2f position = ball->shape.getPosition() + movement;

        if (position.y - BALL_RADIUS < 0) {
            position.y = BALL_RADIUS;
            ball->velocity.y *= -0.8f;
        }
        if (position.y + BALL_RADIUS > windowSize.y) {
            position.y = windowSize.y - BALL_RADIUS;
            ball->velocity.y *= -0.8f;
        }
        if (position.x - BALL_RADIUS < 0) {
            position.x = BALL_RADIUS;
            ball->velocity.x *= -0.8f;
        }
        if (position.x + BALL_RADIUS > windowSize.x) {
            position.x = windowSize.x - BALL_RADIUS;
            ball->velocity.x *= -0.8f;
        }

        ball->shape.setPosition(position);

        for (auto& otherBall : balls) {
            if (ball != otherBall) {
                handleBallCollision(*ball, *otherBall);
            }
        }

        if (stormActive) {
            sf::Vector2f ballPos = ball->shape.getPosition();
            sf::Vector2f mousePos(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
            sf::Vector2f direction = ballPos - mousePos;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (distance > 0) {
                direction /= distance;
                ball->velocity += direction * STORM_FORCE * deltaTime;
            }
        }

        partition.addBall(ball);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Physics Simulation", sf::Style::Close | sf::Style::Resize);

    std::vector<std::shared_ptr<Ball>> balls;
    SpatialPartition partition(GRID_SIZE);
    bool stormActive = false;
    sf::Clock clock;
    float ballGenerationTime = 0.0f;
    bool isFullscreen = false;
    bool isResizing = false;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                isResizing = true;
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::Resized) {
                isResizing = false;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                if (isFullscreen) {
                    window.create(sf::VideoMode(800, 600), "SFML Physics Simulation", sf::Style::Close | sf::Style::Resize);
                } else {
                    window.create(sf::VideoMode::getDesktopMode(), "SFML Physics Simulation", sf::Style::Fullscreen);
                }
                isFullscreen = !isFullscreen;
            }
        }

        if (!isResizing) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                ballGenerationTime -= deltaTime;
                if (ballGenerationTime <= 0) {
                    auto newBall = std::make_shared<Ball>();
                    newBall->shape.setRadius(BALL_RADIUS);
                    newBall->shape.setFillColor(sf::Color::White);
                    newBall->shape.setPosition(static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y));
                    newBall->velocity = sf::Vector2f(0, 0);
                    balls.push_back(newBall);
                    partition.addBall(newBall);
                    ballGenerationTime = BALL_GENERATION_DELAY;
                }
            }
        }

        stormActive = sf::Mouse::isButtonPressed(sf::Mouse::Right);

        updateBalls(balls, deltaTime, stormActive, sf::Mouse::getPosition(window), window.getSize(), partition);

        window.clear(sf::Color::Black);

        for (const auto& ball : balls) {
            window.draw(ball->shape);
        }
        window.display();
    }

    return 0;
}
