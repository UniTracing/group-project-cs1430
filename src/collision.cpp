#include "collision.h"

bool areColliding(Circle& i, point& j) {
    int xDist = j.x - i.p.x;
    int yDist = j.y - i.p.y;
    double distance = getDistance(xDist, yDist);
    double overlap  = i.r - distance;
    return overlap >= 0;
}

bool areColliding(Circle& i, Circle& j) {
    int xDist = j.p.x - i.p.x;
    int yDist = j.p.y - i.p.y;
    double distance = getDistance(xDist, yDist);
    double overlap  = i.r + j.r - distance;
    return overlap >= 0;
}

void checkCollisions(vector<Collision>& collisions, vector<Circle>& group1, vector<Circle>& group2) {
    for (Circle& i : group1) {
        for (Circle& j : group2) {
            if (&i != &j) {
                int xDist = j.p.x - i.p.x;
                int yDist = j.p.y - i.p.y;
                double distance = getDistance(xDist, yDist);
                double overlap  = i.r + j.r - distance;
                if (areColliding(i, j))
                    collisions.push_back((Collision){i, j, xDist, yDist, distance, overlap});
            }
        }
    }
}

void checkCollisions(vector<LineCollision>& collisions, vector<Circle>& circles, vector<Line>& lines) {
    for (Circle& c : circles) {
        for (Line& l : lines) {
            double x1 = l.p1.x, x2 = l.p2.x, x3 = c.p.x,
                   y1 = l.p1.y, y2 = l.p2.y, y3 = c.p.y;
            double determinant = ((x2 - x1) * (y3 - y1)) - ((y2 - y1) * (x3 - x1));
            x3 += cos(c.f.getDirection()) * c.r;
            y3 += sin(c.f.getDirection()) * c.r;
            double nextDeterminant = ((x2 - x1) * (y3 - y1)) - ((y2 - y1) * (x3 - x1));

            bool sameSide = determinant == nextDeterminant || determinant < 0 == nextDeterminant < 0;

            if (!sameSide) {
                double ax = c.p.x  - l.p1.x,
                       bx = l.p2.x - l.p1.x,
                       ay = c.p.y  - l.p1.y,
                       by = l.p2.y - l.p1.y;
                double dot = ax * bx + ay * by;
                double scalar = dot / l.len();

                double minX = min(l.p1.x, l.p2.x);
                double maxX = max(l.p1.x, l.p2.x);
                double minY = min(l.p1.y, l.p2.y);
                double maxY = max(l.p1.y, l.p2.y);

                point proj = {
                    clamp(scalar * (bx / l.len()) + l.p1.x, minX, maxX),
                    clamp(scalar * (by / l.len()) + l.p1.y, minY, maxY)
                };

                if (areColliding(c, proj))
                    collisions.push_back({c, proj, l.angle()});
            }
        }
    }
}

void handleCollisions(vector<Collision>& collisions) {
    while (collisions.size()) {
        Circle& c1      = collisions.back().c1;
        Circle& c2      = collisions.back().c2;
        int xDist       = collisions.back().xDist;
        int yDist       = collisions.back().yDist;
        double distance = collisions.back().distance;
        double overlap  = collisions.back().overlap;

        double xRatio = xDist / distance;
        double yRatio = yDist / distance;

        --c2.health;
        //cout << c2.health << endl;

        c1.p.x -= overlap * xRatio / 2;
        c1.p.y -= overlap * yRatio / 2;
        c2.p.x += overlap * xRatio / 2;
        c2.p.y += overlap * yRatio / 2;

        collisions.pop_back();
    }
}

void handleCollisions(vector<LineCollision>& collisions) {
    while (collisions.size()) {

        Circle& c = collisions.back().c;
        point& p = collisions.back().p;
        double lineAngle = collisions.back().lineAngle;
        double ballDirection = c.f.getDirection() + 3.14;
        double theta = 2 * lineAngle - ballDirection;

        c.f.setDirection(atan2(-sin(theta), -cos(theta)));

        collisions.pop_back();
    }
}
