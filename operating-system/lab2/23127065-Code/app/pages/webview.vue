<script setup lang="ts">
definePageMeta({
  name: 'Webview',
  icon: 'lucide:app-window',
  description: 'Create new webview in a detached window',
  category: 'interface',
});

const { app } = useAppConfig();
const toast = useToast();

const openWindow = async (id: string, page: string) => {
  const webview = new useTauriWebviewWindowWebviewWindow(id, {
    title: 'yupartify webview',
    url: page,
    width: 1280,
    height: 720,
  });

  webview.once('tauri://created', () => {
    toast.add({
      title: 'Success',
      description: 'Webview created',
      color: 'success',
    });
  });
  webview.once('tauri://error', (error) => {
    toast.add({
      title: 'Error',
      description: (error as any).payload,
      color: 'error',
    });
  });
};
</script>

<template>
  <LayoutTile
    title="Webview window"
    description="Create new webview in a detached window. This will create a new window flagged 'secondary' that has the same permissions as the main one. If you need more windows, update the permissions under capabilities > main or create a new capabilities file for the new window only."
  >
    <div class="flex-center flex-wrap gap-6 lg:flex-col">
      <UButton
        variant="subtle"
        @click="openWindow(new Date().valueOf().toString(), app.repo)"
      >
        Create external Webview
      </UButton>
      <UButton variant="subtle" @click="openWindow('secondary', '/os')">
        Create internal Webview
      </UButton>
    </div>
  </LayoutTile>
</template>
