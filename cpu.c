#include "constants.h"
#include "cpu.h"
#include "logging.h"
#include "io.h"
#include "ins.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define DEBOUNCE_MS 150 

CPU cpu;

static uint32_t debounceSTick;

const SDL_Scancode key_map[KEY_NUM] =
{
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

void init()
{
    // Reset pointers, opcodes and timers
    cpu.PC = 0x200;
    cpu.opcode.ins = 0;
    cpu.I = 0;
    cpu.SP = 0;
    cpu.dTimer = 0;
    cpu.sTimer = 0;
    cpu.dFlag = 0;

    // clear stack, reg V0 - VF, display, and initialize mem
    memcpy(cpu.mem, fontset, sizeof(fontset));
    memset(cpu.stack, 0, sizeof(cpu.stack));
    memset(cpu.V, 0, sizeof(cpu.V));
    memset(gfx, ~PIXEL_ON, sizeof(gfx));

    debounceSTick = 0;
    srand(time(NULL));
}

void loadGame(const char *name)
{
    FILE *game = fopen(name, "rb");

    if (game == NULL) {
        const char *err = "Cannot open game file: %s";
        char msg[strlen(err) - 2 + strlen(name) + 1];
        sprintf(msg, err, name);
        logQuit(msg);
    }

    fseek(game, 0L, SEEK_END);
    const size_t BUFFER_SIZE = ftell(game);
    fseek(game, 0L, SEEK_SET);

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    bytesRead = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, game);

    if (bytesRead != BUFFER_SIZE)
        logQuit("Bytes read does not match file size");

    memcpy(&cpu.mem[512], buffer, sizeof(buffer));
}

void updateTimers() {
    if(cpu.dTimer > 0)
        cpu.dTimer--;

    if(cpu.sTimer > 0) {
        if(SDL_GetAudioDeviceStatus(sound) != SDL_AUDIO_PLAYING) {
            SDL_PauseAudioDevice(sound, 0);
            checkSDLError(__LINE__);
        }
        cpu.sTimer--;
    } else if (SDL_GetAudioDeviceStatus(sound) == SDL_AUDIO_PLAYING) {
        SDL_PauseAudioDevice(sound, 1);
        checkSDLError(__LINE__);
    }
}

static uint8_t debouncing()
{
    uint8_t isDebouncing = 0;

    if(debounceSTick > 0)
        if(SDL_GetTicks() - debounceSTick >= DEBOUNCE_MS) {
            debounceSTick = 0;
        } else {
            isDebouncing = 1;
            cpu.PC -= 2;
        }
        
    return isDebouncing;
}

static void setDebounce()
{
    debounceSTick = SDL_GetTicks();
}

static void logOpQuit()
{
    char errMsg[20 + sizeof(cpu.opcode.ins) + 5 + sizeof(cpu.opcode.ins) + 1];
    sprintf(errMsg, "Undefined opcode [0x%04X]: 0x%04X", cpu.opcode.ins & 0xF000, cpu.opcode.ins);
    logQuit(errMsg);
}

void emulateCycle()
{
    // Fetch
    cpu.opcode.ins = cpu.mem[cpu.PC] << 8 | cpu.mem[cpu.PC + 1];
    uint8_t vx = cpu.V[cpu.opcode.x];
    uint8_t vy = cpu.V[cpu.opcode.y];

    // Decode
    switch(cpu.opcode.op)
    {
        case 0x0:
        {
            switch(cpu.opcode.addr)
            {
                case 0x0E0: dispClear(); break;
                case 0x0EE: ret(); break;
                default:
                    logQuit("MC Subroutines are ignored by modern interpreters");
            }
            break;
        }
        case 0x1: jmp(cpu.opcode.addr, "0x1NNN: JMP NNN"); break;
        case 0x2: call(); break;
        case 0x3: skipNextIns(vx == cpu.opcode.kk, "0x3XNN: Skip next instruction if VX==NN"); break;
        case 0x4: skipNextIns(vx != cpu.opcode.kk, "0x4XNN: Skip next instruction if VX!=NN"); break;
        case 0x5: skipNextIns(vx == vy, "0x5XY0: Skip next instruction if VX==VY"); break;
        case 0x6: loadReg(cpu.opcode.x, cpu.opcode.kk, "0x6XNN: VX = NN"); break;
        case 0x7: loadReg(cpu.opcode.x, vx + cpu.opcode.kk, "0x7XNN: VX += NN (No Carry)"); break;
        case 0x8:
        {
            switch(cpu.opcode.n)
            {
                case 0x0: loadReg(cpu.opcode.x, vy, "0x8XY0: VX = VY"); break;
                case 0x1: loadReg(cpu.opcode.x, vx | vy, "0x8XY0: VX = VX | VY"); break;
                case 0x2: loadReg(cpu.opcode.x, vx & vy, "0x8XY2: VX = VX & VY"); break;
                case 0x3: loadReg(cpu.opcode.x, vx ^ vy, "0x8XY3: VX = VX ^ VY"); break;
                case 0x4: addOF(vy, (vy + vx) > 0xFF, "0x8XY4: VX += VY (With VF Carry)"); break;
                case 0x5: addOF(-vy, vx > vy, "0x8XY5: VX -= VY (With VF Borrow)"); break;
                case 0x6: loadMask(LSB8_MASK, "0x8XY6: Store VX LSB in VF LSB, then VX>>=1"); break;
                case 0x7: addOF(vy - vx, vx < vy, "0x8XY7: VX=VY-VX (with VF Borrow)"); break;
                case 0xE: loadMask(MSB8_MASK, "0x8XYE: Store VX MSB in VF LSB, then VX<<=1"); break;
                default:
                    logOpQuit();
            }
            break;
        }
        case 0x9: skipNextIns(vx != vy, "0x9XYN: Skip next instruction if VX!=VY"); break;
        case 0xA: loadI(cpu.opcode.addr, "0xANNN: I = NNN"); break;
        case 0xB: jmp(cpu.V[0] + cpu.opcode.addr, "0xBNNN: PC = V0 + NNN"); cpu.PC -= 2; break;
        case 0xC: loadReg(cpu.opcode.x, (rand() % 0xFF) & cpu.opcode.kk, "0xCXNN: VX = rand() & NNN"); break;
        case 0xD: draw(); break;
        case 0xE:
            switch(cpu.opcode.kk)
            {
                case 0x9E: skipNextIns(SDL_GetKeyboardState(NULL)[key_map[vx]], "0xEX93: Skip next instruction if key()==VX"); break;
                case 0xA1: skipNextIns(!SDL_GetKeyboardState(NULL)[key_map[vx]], "0xEXA1: Skip next instruction if key()!=VX"); break;
                default:
                    logOpQuit();
            }
            break;
        case 0xF:
            switch(cpu.opcode.kk) {
                case 0x07: loadReg(cpu.opcode.x, cpu.dTimer, "0xFX07: VX = dTimer"); break;
                case 0x0A: if(!debouncing() && !loadKeypress()) setDebounce(); break;
                case 0x15: loadTimer(DTIMER, vx, "0xFX15: dTimer = VX"); break;
                case 0x18: loadTimer(STIMER, vx, "0xFX18: sTimer = VX"); break;
                case 0x1E: loadI(cpu.I + vx, "0xFX1E: I += VX (No Carry)"); break;
                case 0x29: loadI(vx * 5, "0xFX29: I = sprite_addr[VX]"); break;
                case 0x33: bcd(); break;
                case 0x55: regMemTrans(cpu.mem, cpu.V, 16, "0xFX55: Store V0 to VX in memory starting at address I"); break;
                case 0x65: regMemTrans(cpu.V, cpu.mem, MEM_SIZE, "0xFX65: Fill V0 to VX in from memory values starting at address I"); break;
                default:
                    logOpQuit();
            }
            break;
        default:
            logOpQuit();
    }

    cpu.PC += 2;
    #ifndef NDEBUG
    dumpRegAndPointerInfo();
    #endif 
} 
