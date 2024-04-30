#-------------------------------------------------------------------------------
# pip install Pillow
# float metallic = texture(metallicMap, TexCoords).b;
# float roughness = texture(roughnessMap, TexCoords).g;
#-------------------------------------------------------------------------------
from PIL import Image

image1 = Image.open("./metallic.png")
image1 = image1.convert("RGB")
image2 = Image.open("./roughness.png")
image2 = image2.convert("RGB")
width, height = image1.size

for x in range(width):
    for y in range(height):
        r1, g, b= image1.getpixel((x,y))
        r2, g, b= image2.getpixel((x,y))
        image1.putpixel((x,y),(0,r2,r1))
image1.save("./metallRoughness.png")

# ------------------------------------------------------------------------------ RadicalInverse_VdC
# import pygame
# pygame.init()

# window_width = 800
# window_height = 600
# screen = pygame.display.set_mode((window_width, window_height))

# slider_width = 200
# slider_height = 20
# slider_x = window_width // 2 - slider_width // 2
# slider_y = window_height // 2 - slider_height // 2
# slider_rect = pygame.Rect(slider_x, slider_y, slider_width, slider_height)

# handle_width = 20
# handle_height = 30
# handle_x = slider_x
# handle_y = slider_y - (handle_height - slider_height) // 2
# handle_rect = pygame.Rect(handle_x, handle_y, handle_width, handle_height)

# font = pygame.font.Font(None, 36)
# dragging = False
# current_n = 0

# def RadicalInverse_VdC(bits):
#     bits = (bits << 16) | (bits >> 16)
#     bits = ((bits & 0x55555555) << 1) | ((bits & 0xAAAAAAAA) >> 1)
#     bits = ((bits & 0x33333333) << 2) | ((bits & 0xCCCCCCCC) >> 2)
#     bits = ((bits & 0x0F0F0F0F) << 4) | ((bits & 0xF0F0F0F0) >> 4)
#     bits = ((bits & 0x00FF00FF) << 8) | ((bits & 0xFF00FF00) >> 8)
#     return bits * 2.3283064365386963e-10

# def Hammersley(i, N):
#     return (float(i) / float(N), RadicalInverse_VdC(i))

# running = True
# while running:
#     for event in pygame.event.get():
#         if event.type == pygame.QUIT:
#             running = False
#         elif event.type == pygame.MOUSEBUTTONDOWN:
#             if event.button == 1 and handle_rect.collidepoint(event.pos):
#                 dragging = True
#         elif event.type == pygame.MOUSEBUTTONUP:
#             if event.button == 1:
#                 dragging = False

#     if dragging:
#         handle_rect.centerx = pygame.mouse.get_pos()[0]
#         handle_rect.clamp_ip(slider_rect)

#         slider_range = slider_width - handle_width
#         normalized_pos = (handle_rect.x - slider_rect.x) / slider_range
#         current_n = int(normalized_pos * 1000) # max of N

#     screen.fill((0, 0, 0))
#     pygame.draw.rect(screen, (255, 255, 255), slider_rect)
#     pygame.draw.rect(screen, (255, 0, 0), handle_rect)

#     text = font.render("N: {}".format(current_n), True, (255, 255, 255))
#     text_rect = text.get_rect(center=(window_width // 2, window_height // 2 + 50))
#     screen.blit(text, text_rect)

#     N = current_n
#     for i in range(N):
#         x, y = Hammersley(i, N)
#         x_pixel = int(x * screen.get_width())
#         y_pixel = int(y * screen.get_height())
#         pygame.draw.circle(screen, (255, 255, 255), (x_pixel, y_pixel), 2)

#     pygame.display.flip()
# pygame.quit()