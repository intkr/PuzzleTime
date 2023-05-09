#include "graphics.h"

extern double rm;

Graphics::Graphics(int w, int h) {
	rm = (double)w / 1920;
	SDL_CreateWindowAndRenderer((int)(1920 * rm), (int)(1080 * rm), SDL_WINDOW_INPUT_FOCUS, &_window, &_renderer);
	SDL_SetWindowTitle(_window, "Puzzle Time");
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	_sprites[0] = &backgroundSprites;
	_sprites[1] = &foregroundSprites;
	_sprites[2] = &popupSprites;
}

Graphics::~Graphics() {
	SDL_DestroyWindow(_window);
	SDL_DestroyRenderer(_renderer);
}

SDL_Window* Graphics::getWindow() {
	return _window;
}

SDL_Renderer* Graphics::getRenderer() {
	return _renderer;
}

std::map<std::string, SDL_Texture*>* Graphics::getTexturePtr() {
	return &_textures;
}

std::map<std::string, Sprite*>** Graphics::getSpritePtr() {
	return _sprites;
}

SDL_Texture* Graphics::getTexture(std::string name) {
	if (_textures.count(name) > 0) {
		return _textures[name];
	}
	else return NULL;
}

bool Graphics::addTexture(std::string path, std::string name) {
	if (_textures.find(name) != _textures.end()) {
		std::cout << "Key \"" << name << "\" already exists in texture data.\n";
		return false;
	}
	SDL_Surface* bgSurface = IMG_Load(path.c_str());
	if (bgSurface == NULL) {
		std::cout << "File path \"" << path << "\" is invalid, failed to add texture \"" << name << "\".\n";
		return false;
	}
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(_renderer, bgSurface);
	SDL_SetTextureBlendMode(bgTexture, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(bgSurface);
	_textures[name] = bgTexture;
	return true;
}

bool Graphics::addSprite(SDL_Texture* tex, SDL_Rect* src, SDL_FRect* dst, int type, std::string name, double a) {
	if (tex == NULL) {
		std::cout << "Null texture, adding sprite \"" << name << "\" failed.\n";
		return false;
	}
	if (type < BACKGROUND || type > POPUP) {
		std::cout << "Invalid sprite type, adding sprite \"" << name << "\" failed.\n";
		return false;
	}
	if ((*_sprites[type]).count(name) > 0) {
		std::cout << "Name already exists in sprite data, adding sprite \"" << name << "\" failed.\n";
		return false;
	}

	(*_sprites[type])[name] = new Sprite(tex, src, dst, a);
	return true;
}

bool Graphics::hasSprites() {
	for (int i = 0; i < 3; i++) {
		if (_sprites[i]->size() > 0) return false;
	}
	return true;
}

bool Graphics::doesPopupExist() {
	return popupSprites.size() > 0;
}

void Graphics::triggerOutro() {
	for (int i = 0; i < 3; i++) {
		for (auto sprite : *_sprites[i]) {
			sprite.second->triggerOutro();
		}
	}
}

void Graphics::renderScreen() {
	SDL_RenderClear(_renderer);

	// render all sprites - background -> foreground -> popup
	for (int i = 0; i < 3; i++) {
		// If there's a pop-up window to render, cover the window with an translucent black sprite.
		if (i == 2 && doesPopupExist()) {
			SDL_Texture* t = SDL_CreateTexture(_renderer, 0, SDL_TEXTUREACCESS_TARGET, 1, 1);
			SDL_SetRenderTarget(_renderer, t);
			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 128);
			SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
			SDL_RenderDrawPoint(_renderer, 0, 0);
			SDL_SetRenderTarget(_renderer, NULL);
			SDL_RenderCopy(_renderer, t, NULL, NULL);
			SDL_DestroyTexture(t);
		}
		/*
		Small note - read if screen flickers black when changing states

		It happens because when triggerOutro is triggered, updateSprite will return false if there is no outro animation to play.
		End of (or lack of) animation means that the sprite will not be drawn. This results in a black screen.
		*/
		for (auto iter = (*_sprites[i]).cbegin(); iter != (*_sprites[i]).cend();) {
			Sprite* s = iter->second;
			bool validSprite = s->updateSprite();
			if (validSprite) {
				if (s->isVisible()) SDL_RenderCopyExF(_renderer, s->getTexture(), s->getSrcRect(), s->getDstRect(), s->getAngle(), NULL, SDL_FLIP_NONE);
				iter++;
			}
			else {
				delete s;
				_sprites[i]->erase(iter++);
			}
		}
	}

	SDL_RenderPresent(_renderer);
}

void Graphics::reset() {
	emptySprites();
	emptyTextures();
}

void Graphics::emptySprites() {
	for (int i = 0; i < 3; i++) {
		for (auto it = _sprites[i]->begin(); it != _sprites[i]->end();) {
			delete it->second;
			it = _sprites[i]->erase(it);
		}
		_sprites[i]->clear();
	}
}

void Graphics::emptyTextures() {
	for (auto it = _textures.begin(); it != _textures.end();) {
		SDL_DestroyTexture(it->second);
		_textures.erase(it++);
	}
}

bool Graphics::addAnimation(std::string spriteName, std::string aniName, Animation* a, int type) {
	int index = -1;
	for (int i = 0; i < 3; i++) {
		if ((*_sprites[i]).count(spriteName) > 0) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		std::cout << "Sprite \"" << spriteName << "\" doesn't exist, failed to add animation.\n";
		return false;
	}
	
	(*_sprites[index])[spriteName]->addAnimation(aniName, a, type);
	return true;
}
