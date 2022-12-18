#include "particle.h"

//생성자
ParticleGenerator::ParticleGenerator() {
}
ParticleGenerator::ParticleGenerator(float x, float y, float z) {
}

//위치&변수셋
void ParticleGenerator::setPos(float x, float y, float z) {
    alive = true;
    pos.set(x, y, z);            //내위치 셋

 //   float velRan = 0.4f;
    float velRan = 0.2f;

    for (int i = 0; i < PARTICLE_MAX; i++) {
        particles[i].alive = true;               //변수 셋팅
  //      particles[i].color.set(1.0f, 0.3f, 0.3f);   //빨강
        particles[i].color.set(0.5f, 0.9f, 0.5f);   //분홍
  //      particles[i].color.set(0.9f, 0.9f, 0.0f);   //노랑
  //      particles[i].color.set(0.0f, 1.0f, 1.0f);   //하늘
        particles[i].pos.set(x, y, z);
        particles[i].size = 5;
        particles[i].lifetime = particles[i].maxLifetime = 30;   //수명

        particles[i].vel.set(random(-velRan, velRan), random(-velRan, velRan), random(-velRan, velRan));
        particles[i].acc = particles[i].vel * -0.06;
    }
}

//파티클출력
void ParticleGenerator::DrawParticle() {
    bool drawing = false;

    for (int i = 0; i < PARTICLE_MAX; i++) {   //파티클배열
        if (particles[i].alive == true) {//살아있으면
            drawing = true;

            //남은 수명에 따라 투명하게
            float alpha = (float)particles[i].lifetime / particles[i].maxLifetime;

            //파티클 그림
            glPointSize(particles[i].size);
            glColor4f(particles[i].color.x, particles[i].color.y, particles[i].color.z, alpha);
            glBegin(GL_POINTS);
            glVertex3f(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
            glEnd();

            //후처리
            particles[i].lifetime -= 1;      //수명-1
            if (particles[i].lifetime <= 0) {
                particles[i].alive = false;
            }

            //         particles[i].acc = particles[i].vel * -0.14f;      //가속도         
            particles[i].vel += particles[i].acc;      //속도변화
   //         particles[i].vel = particles[i].vel * 0.95;   //속도감소
            particles[i].pos += particles[i].vel;      //속도변화
        }
    }

    if (drawing == false) {//파티클 없으면 비활성화
        alive = false;
    }
}
